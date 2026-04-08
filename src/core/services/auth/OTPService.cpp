#include "OTPService.h"
#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../utils/Utils.h"

/* --------------  OTPService Implementation ---------------------  */
OTPService::OTPService(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

int OTPService::sendOTP(const QString &email) {
    QJsonObject obj;
    obj["email"] = email;

    QJsonDocument doc(obj);

    // POST request of OTP
    QNetworkReply *reply = manager->post(configureNetworkRequest(route(APIRoutes::OTP_SEND)), doc.toJson());

    // Wait until we get final response from the server
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // If there's a problem in connectivity
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Failed to Send OTP. " << reply->errorString();
        return -1;
    }

    // Reading response received from the server
    QByteArray response = reply->readAll();
    QJsonDocument r = QJsonDocument::fromJson(response);

    if (!r.isObject()) {
        qDebug() << "Json is Invalid.";
        return -1;
    }

    // Collecting Response Data
    QJsonObject rObj = r.object();
    status_code = rObj["status_code"].toInt();
    message = rObj["message"].toString();

    qDebug() << "OTPService.cpp::51  " << status_code << "  " << message;

    reply->deleteLater();
    return status_code;
}

std::expected<bool, OTPService::OTPError> OTPService::verifyOTP(const QString &otp, const QString &email) {
    QJsonObject obj { {"email", email}, {"otp", otp} };
    QJsonDocument doc(obj);

    QNetworkReply *reply = manager->post(configureNetworkRequest(route(APIRoutes::OTP_VERIFY)), doc.toJson());

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // Network error
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error:" << reply->errorString();
        OTPError err { -1, "Network Request Failed." };
        reply->deleteLater();
        return std::unexpected(err);
    }

    QByteArray response = reply->readAll();
    QJsonDocument r = QJsonDocument::fromJson(response);

    if (!r.isObject()) {
        qDebug() << "JSON is not valid";
        reply->deleteLater();
        return std::unexpected(OTPError{-1, "Invalid JSON response"});
    }

    // Getting Main Json Object
    QJsonObject rObj = r.object();

    // Server error response
    if (rObj.contains("status_code") && rObj.contains("message")) {
        OTPError err { rObj["status_code"].toInt(), rObj["message"].toString() };
        reply->deleteLater();
        return std::unexpected(err);
    }

    // OTP verification success
    bool verified = rObj["isVerified"].toBool();

    qDebug() << "OTPService.cpp::102   " << "OTP Verified: -> " << verified;

    reply->deleteLater();
    return verified;
}

QNetworkRequest OTPService::configureNetworkRequest(const QString &route) {
    QUrl url(route);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("api_key", API_KEY.toUtf8());
    request.setRawHeader("accept", "application/json");
    request.setTransferTimeout(REQUEST_TIMEOUT);
    return request;
}

/* ----------------------  Get OTP  ---------------------------- */
GetOTP::GetOTP(AuthWindow *ac, QObject *parent) : QObject(parent) {
    ov = new OTPService(this); // Initializing OTP Validator class

    timer = new QTimer(this); // Initializing OTP Timer
    connect(timer, &QTimer::timeout, this, &GetOTP::onTimeout);

    // Error Dialog Manager
    errorManager = ErrorDialogManager::instance();
}

bool GetOTP::setAccountOtp(Otp *instance, const QString &email) {
    if (!instance)  
        return false;
    
    ao = instance;

    // Store user email address
    cEmail = email;

    // Update OTP UI with masked email
    ao->setEmail(cEmail);

    // Connect Otp UI signals → GetOTP logic
    connect(ao, &Otp::verifyClicked, this, &GetOTP::onVerifyClicked);
    connect(ao, &Otp::cancelClicked, this, &GetOTP::onCancelClicked);
    connect(ao, &Otp::resendClicked, this, &GetOTP::onResendClicked);

    // When OTP input fields are filled, store the code
    connect(ao->otpWidget(), &OTPWidget::OTPcompleted, this, [this](const QString &otp){ 
        ao->verifyButton()->setEnabled(true);
        currOtp = otp; 
    });

    // Connect resend-limit signal once
    connect(this, &GetOTP::maxLimitReached, this, &GetOTP::onMaxLimitReached);

    // Connect when get something wrong
    connect(this, &GetOTP::somethingWrong, this, &GetOTP::onSomethingWrong);

    return handleSentOTPResponse(); 
}

bool GetOTP::handleSentOTPResponse() {
    // Send OTP to user
    int status_code = ov->sendOTP(cEmail);

    switch (status_code) {
        case 200: // OTP Sent and Timer get started
            resendOtpWithTimer();
            break;

        case 429: // Max Resend Limit Reached
            emit maxLimitReached();
            return false;
            break;

        // Other Server Errors (Refer to backend exceptions docs)
        case 500:
        case 512:
        case 400:
        case -1: 
            emit somethingWrong();
            return false;
            break;
    }

    return true;
}

using IC = Utils::InternetConnectivity;

void GetOTP::onResendClicked() {
    IC::instance().runIfOnline([this](){
        handleSentOTPResponse();
    }, this, errorManager);
}

void GetOTP::onMaxLimitReached() {
    disableControls("Verify");

    if (ao->message())
        ao->message()->setAnimatedText("Maximum limit reached. Try again after 24hrs.");
}

void GetOTP::onVerifyClicked() {
    if (!ao)
        return;
        
    IC::instance().runIfOnline([this](){
        ao->verifyButton()->setEnabled(false);
        ao->verifyButton()->setText(""); // Loader starts

        auto response = ov->verifyOTP(currOtp, cEmail);

        if (response) {
            bool isVerified = *response;

            if (isVerified) {
                disableControls("Verified");
            } else {
                ao->verifyButton()->setText("Verify");
                ao->verifyButton()->setEnabled(true);
                ao->message()->setAnimatedText("Your entered OTP is incorrect or expired.");
            } 

            emit OTPVerified(isVerified);

        } else {
            const auto &error = response.error();
            qDebug() << "OTPService.cpp::227  " << error.status_code << " and " << error.message;

            switch (error.status_code) {
                case -1:
                case 500:
                    ao->message()->setAnimatedText("Failed to verify your entered OTP.");
                    ao->verifyButton()->setText("Verify");   
                    ao->verifyButton()->setEnabled(true);     
                    break;

                case 403:
                    emit maxLimitReached();
                    break;

                case 401:
                    emit somethingWrong();
                    break;
            }
        }
    }, this, errorManager);
}

void GetOTP::onCancelClicked() const { QApplication::quit(); }

void GetOTP::onTimeout() {
    totalSecs--;

    if (totalSecs < 0) {
        timer->stop();
        ao->resendOtpWidget()->button()->setEnabled(true);
        ao->resendOtpWidget()->timer()->hide();
        return;
    }

    int mins = totalSecs / 60;
    int secs = totalSecs % 60;

    timeString = QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
    ao->resendOtpWidget()->timer()->setText(timeString);
}

void GetOTP::onSomethingWrong() { 
    errorManager->show("SomethingWentWrong", "Auth"); // Show error dialog if there is an error received from server
    ao->setEmail("");
    disableControls("Verify");                // Disable verify button
}

void GetOTP::disableControls(QString btnText) {
    ao->verifyButton()->setText(btnText);
    ao->verifyButton()->setEnabled(false);              // Disables the verify button after successful OTP matching
    ao->resendOtpWidget()->button()->setEnabled(false); // Disables the resend button when otp is verified
    ao->resendOtpWidget()->timer()->hide();             // Hides the timer
    ao->message()->setAnimatedText("");

    if (timer) 
        timer->stop();

    ao->otpWidget()->setEnabled(false);
}

void GetOTP::resendOtpWithTimer() {
    ao->resendOtpWidget()->button()->setEnabled(false);
    ao->resendOtpWidget()->timer()->setText(OTP_INITIAL_TIME_STR); // Initial time
    ao->resendOtpWidget()->timer()->show();
    
    totalSecs = OTP_TIME;

    if (timer) 
        timer->start(1000); // 1 sec
}