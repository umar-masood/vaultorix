#include "OTP.h"

#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../utils/Utils.h"
#include "../../../ui/auth/auth_window/AuthWindow.h"
#include "../../../ui/auth/otp/Otp.h"
#include "../../../ui/dialogs/error_dialog/ErrorDialog.h"

using Core::Services::Auth::OTP;
using Core::Services::Auth::GetOTP;
using IC = Utils::InternetConnectivity;
using namespace Ui::Auth;

/* --------------  OTP Implementation ---------------------  */
OTP::OTP(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

int OTP::sendOTP(const QString &email) {
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

    qDebug() << "OTP.cpp::47  " << status_code << "  " << message;

    reply->deleteLater();
    return status_code;
}

std::expected<bool, OTP::OTPError> OTP::verifyOTP(const QString &otp, const QString &email) {
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

    qDebug() << "OTP.cpp::93   " << "OTP Verified: -> " << verified;

    reply->deleteLater();
    return verified;
}

QNetworkRequest OTP::configureNetworkRequest(const QString &route) {
    QUrl url(route);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("api_key", API_KEY.toUtf8());
    request.setRawHeader("accept", "application/json");
    request.setTransferTimeout(REQUEST_TIMEOUT);
    return request;
}

/* ----------------------  Get OTP  ---------------------------- */
GetOTP::GetOTP(Ui::Auth::AuthWindow *authWindow, QObject *parent) : QObject(parent) {
    otpValidator = new OTP(this); // Initializing OTP Validator class

    timer = new QTimer(this); // Initializing OTP Timer
    connect(timer, &QTimer::timeout, this, &GetOTP::onTimeout);

    // Error Dialog Manager
    errorManager = ErrorDialogManager::instance();
}

bool GetOTP::setAccountOTPWidget(Ui::Auth::Otp *instance, const QString &email) {
    if (!instance)  
        return false;
    
    accountOTPWidget = instance;

    // Store user email address
    cEmail = email;

    // Update OTP UI with masked email
    accountOTPWidget->setEmail(cEmail);

    // Connect Otp UI signals → GetOTP logic
    connect(accountOTPWidget, &Ui::Auth::Otp::verifyClicked, this, &GetOTP::onVerifyClicked);
    connect(accountOTPWidget, &Ui::Auth::Otp::cancelClicked, this, &GetOTP::onCancelClicked);
    connect(accountOTPWidget, &Ui::Auth::Otp::resendClicked, this, &GetOTP::onResendClicked);

    // When OTP input fields are filled, store the code
    connect(accountOTPWidget->otpWidget(), &OTPWidget::OTPcompleted, this, [this](const QString &otp){ 
        accountOTPWidget->verifyButton()->setEnabled(true);
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
    int status_code = otpValidator->sendOTP(cEmail);

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

void GetOTP::onResendClicked() {
    IC::instance().runIfOnline([this](){
        handleSentOTPResponse();
    }, this, errorManager);
}

void GetOTP::onMaxLimitReached() {
    disableControls("Verify");

    if (accountOTPWidget->message())
        accountOTPWidget->message()->setAnimatedText("Maximum limit reached. Try again after 24hrs.");
}

void GetOTP::onVerifyClicked() {
    if (!accountOTPWidget)
        return;
        
    IC::instance().runIfOnline([this](){
        accountOTPWidget->verifyButton()->setEnabled(false);
        accountOTPWidget->verifyButton()->setText(""); // Loader starts

        auto response = otpValidator->verifyOTP(currOtp, cEmail);

        if (response) {
            bool isVerified = *response;

            if (isVerified) {
                disableControls("Verified");
            } else {
                accountOTPWidget->verifyButton()->setText("Verify");
                accountOTPWidget->verifyButton()->setEnabled(true);
                accountOTPWidget->message()->setAnimatedText("Your entered OTP is incorrect or expired.");
            } 

            emit OTPVerified(isVerified);

        } else {
            const auto &error = response.error();
            qDebug() << "OTP.cpp::217  " << error.status_code << " and " << error.message;

            switch (error.status_code) {
                case -1:
                case 500:
                    accountOTPWidget->message()->setAnimatedText("Failed to verify your entered OTP.");
                    accountOTPWidget->verifyButton()->setText("Verify");   
                    accountOTPWidget->verifyButton()->setEnabled(true);     
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
        accountOTPWidget->resendOtpWidget()->button()->setEnabled(true);
        accountOTPWidget->resendOtpWidget()->timer()->hide();
        return;
    }

    int mins = totalSecs / 60;
    int secs = totalSecs % 60;

    timeString = QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
    accountOTPWidget->resendOtpWidget()->timer()->setText(timeString);
}

void GetOTP::onSomethingWrong() { 
    errorManager->show("SomethingWentWrong", "Auth"); // Show error dialog if there is an error received from server
    accountOTPWidget->setEmail("");
    disableControls("Verify");                        // Disable verify button
}

void GetOTP::disableControls(QString btnText) {
    accountOTPWidget->verifyButton()->setText(btnText);
    accountOTPWidget->verifyButton()->setEnabled(false);              // Disables the verify button after successful OTP matching
    accountOTPWidget->resendOtpWidget()->button()->setEnabled(false); // Disables the resend button when otp is verified
    accountOTPWidget->resendOtpWidget()->timer()->hide();             // Hides the timer
    accountOTPWidget->message()->setAnimatedText("");

    if (timer) 
        timer->stop();

    accountOTPWidget->otpWidget()->setEnabled(false);
}

void GetOTP::resendOtpWithTimer() {
    accountOTPWidget->resendOtpWidget()->button()->setEnabled(false);
    accountOTPWidget->resendOtpWidget()->timer()->setText(OTP_INITIAL_TIME_STR); // Initial time
    accountOTPWidget->resendOtpWidget()->timer()->show();
    
    totalSecs = OTP_TIME;

    if (timer) 
        timer->start(1000); // 1 sec
}