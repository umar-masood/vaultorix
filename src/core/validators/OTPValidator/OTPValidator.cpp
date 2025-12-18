#include "OTPValidator.h"

/* OTPValidator Implementation */
OTPValidator::OTPValidator(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

int OTPValidator::sendOTP(const QString &fullName, const QString &username, const QString &email) {
    QNetworkRequest request(API_URL + "sendOtp");
    
    // Headers of Request
    request.setRawHeader("accept", "application/json");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("api_key", API_KEY.toUtf8());

    // Send OTP Data for POST request to the server
    QJsonObject obj;
    obj["name"] = fullName;
    obj["username"] = username;
    obj["email"] = email;

    QJsonDocument doc(obj);

    // POST request of OTP
    QNetworkReply *reply = manager->post(request, doc.toJson());

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

    qDebug() << status_code << ",  " << message;

    reply->deleteLater();
    return status_code;
}

bool OTPValidator::verifyOTP(const QString &otp, const QString &email)
{
    QNetworkRequest request(API_URL + "verifyOtp");
    request.setRawHeader("accept", "application/json");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Verification OTP Data
    QJsonObject obj;
    obj["email"] = email;
    obj["otp"] = otp;

    QJsonDocument doc(obj);

    QNetworkReply *reply = manager->post(request, doc.toJson());
    QEventLoop loop;
    
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // If there's an error
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error:  " << reply->errorString();
        return false;
    }

    QByteArray response = reply->readAll();
    QJsonDocument r = QJsonDocument::fromJson(response);

    // Make sure the response has main object for valid JSON
    if (!r.isObject()) {
        qDebug() << "Json is not valid";
        return false;
    }

    QJsonObject rObj = r.object();
    isVerified = rObj["isVerified"].toBool();

    qDebug() << "OTP Verified: " << isVerified;

    reply->deleteLater();
    return isVerified; // If the OTP is verified then it will return True otherwise False
}

/* GetOTP Implementation */
GetOTP::GetOTP(QObject *parent) : QObject(parent) {
    ov = new OTPValidator(this); // Initializing OTP Validator class

    timer = new QTimer(this); // Initializing OTP Timer
    connect(timer, &QTimer::timeout, this, &GetOTP::onTimeout);
}

bool GetOTP::setAccountOTPObjectWithDetails(AccountOTP *ao, QString &email, QString &username, QString &name) {
    if (!ao)  return false;
    this->ao = ao;

    // Store user details
    cEmail = email; cUserName = username; cName = name;

    // Clear original variables (as you intended)
    email = username = name = "";

    // Update OTP UI with masked email
    ao->setEmail(cEmail);

    // Connect AccountOTP UI signals → GetOTP logic
    connect(ao, &AccountOTP::verifyClicked, this, &GetOTP::onVerifyClicked);
    connect(ao, &AccountOTP::cancelClicked, this, &GetOTP::onCancelClicked);
    connect(ao, &AccountOTP::resendClicked, this, &GetOTP::onResendClicked);

    // When OTP input fields are filled, store the code
    connect(ao->OTP(), &OTPWidget::OTPcompleted, this, [this](const QString &otp){ currOtp = otp; });

    // Connect resend-limit signal once
    connect(this, &GetOTP::maxLimitReached, this, &GetOTP::onMaxLimitReached);

    // Connect when get something wrong
    connect(this, &GetOTP::somethingWrong, this, &GetOTP::onSomethingWrong);

    // Send OTP to user
    int status_code = ov->sendOTP(cName, cUserName, cEmail);

    // If otp send sucess
    if (status_code == 200)
        resendOtpWithTimer();
    
    // If max resend limit reached (HTTP 429)
    if (status_code == 429)  {
        emit maxLimitReached();
        return false;
    }
    
    // If some server error occurs
    if (status_code == 500 || status_code == 502 || status_code == 512 || status_code == 400 || status_code == -1) {
        emit somethingWrong();
        return false;
    }

    return true; // OTP sent successfully
}

void GetOTP::onResendClicked() {
    int code = ov->sendOTP(cName, cUserName, cEmail);
    if (code == 429) {
        emit maxLimitReached();
        return;
    }

    // In case if we got error from server after clicking on resend button
    if (code == 500 || code == 502 || code == -1 || code == 400 || code == 512) {
        emit somethingWrong();
        return;
    }

    // When user clicks resend, resend button will disabled and timer shows and start
    if (code == 200)
        resendOtpWithTimer();
}

void GetOTP::onMaxLimitReached() {
    if (ao && ao->resendOtpWidget()) {
        ao->resendOtpWidget()->btn()->setEnabled(false); // Get the resend button inside textwithbutton & disable it
        ao->resendOtpWidget()->timerLabel()->hide(); // Hide the timer label inside textwithbutton
    }

    if (ao && ao->OTP()) 
        ao->OTP()->setEnabled(false); // Disables the OTP widget 
    

    if (ao && ao->verifyBtn()) 
        ao->verifyBtn()->setEnabled(false); // Disables the verify button
    

    if (ao && ao->messageLabel()) // Display the maximum limit reached message.
        ao->messageLabel()->setTextAnimated("Maximum limit reached. Try again after 48hrs.");
}

void GetOTP::onVerifyClicked() {
    ao->verifyBtn()->setEnabled(false);
    ao->verifyBtn()->setText("Verifying...");

    bool ok = ov->verifyOTP(currOtp, cEmail);

    if (!ok) {  // If the otp is incorrect or expired
        ao->verifyBtn()->setText("Verify");
        ao->verifyBtn()->setEnabled(true);
        ao->messageLabel()->setTextAnimated("Your entered OTP is incorrect or expired.");
    } else {
        disableControls("Verified");
    }
    emit otpVerified(ok);
}

void GetOTP::onCancelClicked() { QApplication::quit(); }

void GetOTP::onTimeout() {
    totalSecs--;

    if (totalSecs < 0) {
        timer->stop();
        ao->resendOtpWidget()->btn()->setEnabled(true);
        ao->resendOtpWidget()->timerLabel()->hide();
        return;
    }

    int mins = totalSecs / 60;
    int secs = totalSecs % 60;

    timeString = QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
    ao->resendOtpWidget()->timerLabel()->setText(timeString);
}

void GetOTP::onSomethingWrong() {
    disableControls("Verify");
}

void GetOTP::disableControls(QString btnText) {
    ao->verifyBtn()->setText(btnText);
    ao->verifyBtn()->setEnabled(false);              // Disabled the verify button after successful OTP matching
    ao->resendOtpWidget()->btn()->setEnabled(false); // Disabled the resend button when otp is verified
    ao->resendOtpWidget()->timerLabel()->hide();     // hides the timer
    ao->messageLabel()->setTextAnimated("");

    if (timer) timer->stop();
    ao->OTP()->setEnabled(false);
}

void GetOTP::resendOtpWithTimer() {
    ao->resendOtpWidget()->btn()->setEnabled(false);
    ao->resendOtpWidget()->timerLabel()->setText("01:30"); // Initial time
    ao->resendOtpWidget()->timerLabel()->show();
    totalSecs = 90;
    if (timer) timer->start(1000);
}
