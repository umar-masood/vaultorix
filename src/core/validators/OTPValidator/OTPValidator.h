#pragma once

#include <QNetworkAccessManager>
#include <QApplication>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QTimer>
#include <QChar>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

#include <iostream>

#include "../../../ui/accountWindow/AccountWindow.h"
#include "../../../ui/accountOTP/AccountOTP.h"
#include "../../../ui/dialogs/errorDialog/ErrorDialog.h"

/* ------------------ OTP Validator ------------- */
class OTPValidator : public QObject {
    Q_OBJECT

    public:
    OTPValidator(QObject *parent = nullptr);
    int sendOTP(const QString &fullName, const QString &username, const QString &email);
    bool verifyOTP(const QString &otp, const QString &email, const QString &username);

    private:
    // Network Manager
    QNetworkAccessManager *manager = nullptr;

    // API Credentials
    // const QString API_URL = "https://www.umarcreations.site/"; for testing
    const QString API_URL = "http://127.0.0.1:8000/";
    const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";
    
    // API Response
    int status_code;
    QString message;    

    // Verification flag for OTP
    bool isVerified = false;

};

/* ----------------------  Get OTP ------------------------ */
class GetOTP : public QObject {
    Q_OBJECT

    public:
    explicit GetOTP(AccountWindow *ac = nullptr, QObject *parent = nullptr);
    bool setAccountOTPObjectWithDetails(AccountOTP *ao, const QString &email, const QString &username, const QString &name);

    signals:
    void OTPVerified(bool isVerified); // Emits verification result: True = verified
    void maxLimitReached();       // Emits when resend limit (HTTP 429) reached
    void somethingWrong();       // Emits when (HTTP 400) got

    private:
    // Current Account OTP 
    AccountOTP *ao = nullptr;

    // OTP Validator
    OTPValidator *ov = nullptr;

    // Stores email, username, current otp and name from textfields
    QString cEmail, cUserName, cName;
    QString currOtp;

    // OTP Expiry
    int totalSecs = 90;
    QTimer *timer = nullptr;
    QString timeString;

    // Error Dialog
    ErrorDialogManager *errorManager = nullptr;
    
    // Helper Methods
    void resendOtpWithTimer();
    void disableControls(QString btnText);

    // Slots
    void onResendClicked();
    void onMaxLimitReached();
    void onVerifyClicked();
    void onCancelClicked() const;
    void onTimeout();
    void onSomethingWrong();
};
