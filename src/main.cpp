#include <iostream>

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QTimer>
#include <QString>
#include <QChar>
#include <QObject>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QEventLoop>

#include "ui/accountWindow/AccountWindow.h"
#include "ui/accountOTP/AccountOTP.h"

class OTPValidator {
public:
    OTPValidator() {}

    // To send OTP to the user
    int sendOTP(const QString &fullName, const QString &username, const QString &email)
    {
        QNetworkRequest request(API_URL + "sendOtp");
        
        // Headers of Request
        request.setRawHeader("accept", "application/json");
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // Send OTP Data for POST request to the server
        QJsonObject obj;
        obj["name"] = fullName;
        obj["username"] = username;
        obj["email"] = email;

        QJsonDocument doc(obj);

        // POST request of OTP
        QNetworkReply *reply = manager.post(request, doc.toJson());

        // Wait until we get final response from the server
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        // If there's a problem in connectivity
        if (reply->error() != QNetworkReply::NoError)
        {
            std::cerr << "Failed to send otp..." << reply->errorString().toStdString();
            return -1;
        }

        // Reading response received from the server
        QByteArray response = reply->readAll();
        QJsonDocument r = QJsonDocument::fromJson(response);
        if (!r.isObject())
            std::cerr << "Sending Response is invalid...";

        // Collecting Response Data
        QJsonObject rObj = r.object();
        status_code = rObj["status_code"].toInt();
        message = rObj["message"].toString();
        qDebug() << status_code << ",  " << message;

        reply->deleteLater();
        return status_code;
    }

    bool verifyOTP(const QString &otp, const QString &email)
    {
        QNetworkRequest request(API_URL + "verifyOtp");
        request.setRawHeader("accept", "application/json");
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // Verification OTP Data
        QJsonObject obj;
        obj["email"] = email;
        obj["otp"] = otp;

        QJsonDocument doc(obj);

        QNetworkReply *reply = manager.post(request, doc.toJson());
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        // If there's an error
        if (reply->error() != QNetworkReply::NoError)
        {
            std::cerr << "We could not be able to verify your entered OTP" << reply->errorString().toStdString();
            return false;
        }

        QByteArray response = reply->readAll();
        QJsonDocument r = QJsonDocument::fromJson(response);

        // Make sure the response has main object for valid JSON
        if (!r.isObject())
        {
            std::cerr << "Verification Response is invalid...";
            return false;
        }

        QJsonObject rObj = r.object();
        isVerified = rObj["isVerified"].toBool();
        reply->deleteLater();

        return isVerified; // If the OTP is verified then it will return True otherwise False
    }

private:
    QNetworkAccessManager manager;
    const QString API_URL = "https://www.umarcreations.site/";
    int status_code;
    bool isVerified = false;
    QString message;
};

class GetOTP : public QObject
{
    Q_OBJECT

public:
    explicit GetOTP(QObject *parent = nullptr) : QObject(parent) {
        ov = new OTPValidator; // Initializing OTP Validator class
    }

    /*
        This method MUST be called right after creating AccountOTP,
        and it MUST receive the same instance of AccountOTP that appears in UI.

        It performs the following tasks:

        1. Stores the user details (email, username, name).
        2. Updates the AccountOTP UI with the user's masked email.
        3. Sends the OTP to the user using OTPValidator.
        4. Connects all AccountOTP signals to the proper slots (only once).
        5. Emits 'maxLimitReached' when the server returns status 429
           indicating that resend attempts are exhausted.
    */
    bool setAccountOTPObjectWithDetails(AccountOTP *ao, QString &email, QString &username, QString &name) {
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
        connect(ao->OTP(), &OTPWidget::OTPcompleted, this, [this](const QString &otp){
            currOtp = otp; });

        // Connect resend-limit signal once
        connect(this, &GetOTP::maxLimitReached, this, &GetOTP::onMaxLimitReached);

        // Send OTP to user
        int status_code = ov->sendOTP(cName, cUserName, cEmail);

        // If max resend limit reached (HTTP 429)
        if (status_code == 429)
            emit maxLimitReached();

        // Any failure returns false
        if (status_code == 429 || status_code == 500 || status_code == 502 || status_code == -1)
            return false;

        return true; // OTP sent successfully
    }

signals:
    void otpVerified(bool value); // Emits verification result: True = verified
    void maxLimitReached();       // Emits when resend limit (HTTP 429) reached

private slots:

    /*
        Called when the user clicks "Resend".
        - Sends OTP again.
        - If server returns status 429, disable resend button.
    */
    void onResendClicked()
    {
        int code = ov->sendOTP(cName, cUserName, cEmail);
        if (code == 429) {
            emit maxLimitReached();
            return;
        }
    }

    /*
        Called when server reports resend limit exceeded.
        This slot disables the resend button in the AccountOTP UI.
    */
    void onMaxLimitReached()
    {
        if (ao && ao->resendBtn())
            ao->resendBtn()->setEnabled(false);
    }

    /*
        Called when user clicks "Verify".
        - Shows "Verifying..."
        - Calls OTP verification API
        - Updates button text and state accordingly
    */
    void onVerifyClicked()
    {
        ao->verifyBtn()->setEnabled(false);
        ao->verifyBtn()->setText("Verifying...");

        bool ok = ov->verifyOTP(currOtp, cEmail);

        if (!ok) {  // If the otp is incorrect or expired
            ao->verifyBtn()->setText("Verify");
            ao->verifyBtn()->setEnabled(true);
        } else {
            ao->verifyBtn()->setText("Verified");
            ao->verifyBtn()->setEnabled(false); // Disabled the verify button after successful OTP matching
            ao->resendBtn()->setEnabled(false); // Disabled the resend button when otp is verified
        }

        emit otpVerified(ok);
    }

    void onCancelClicked() {
        QApplication::quit();
    }

private:
    AccountOTP *ao = nullptr;   // Pointer to OTP UI instance
    OTPValidator *ov = nullptr; // Handles API calls

    QString cEmail, cUserName, cName; // User details
    QString currOtp;                  // Stores the currently entered OTP
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AccountWindow *aw = new AccountWindow;
    AccountOTP *ac = new AccountOTP;
    aw->setRightWidget(ac);

    GetOTP *go = new GetOTP;
    QString e = "alimasood7281@gmail.com", n = "Umar Masood", u = "umar123";
    qDebug() << go->setAccountOTPObjectWithDetails(ac, e, u, n);

    QObject::connect(go, &GetOTP::otpVerified, [](bool value)
                     { qDebug() << "Verify:  " << value; });


    aw->show();
    return app.exec();
}
#include "main.moc"