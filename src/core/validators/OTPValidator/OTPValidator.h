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
#include "../../../ui/accountOTP/AccountOTP.h"

/* ------------------ OTP Validator ------------- */
class OTPValidator : public QObject {
    Q_OBJECT

    public:
    OTPValidator(QObject *parent = nullptr);
    int sendOTP(const QString &fullName, const QString &username, const QString &email);
    bool verifyOTP(const QString &otp, const QString &email);

    private:
    // Network Manager
    QNetworkAccessManager *manager = nullptr;

    // API Credentials
    const QString API_URL = "https://www.umarcreations.site/";
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
    explicit GetOTP(QObject *parent = nullptr);
    bool setAccountOTPObjectWithDetails(AccountOTP *ao, const QString &email, const QString &username, const QString &name);

    signals:
    void otpVerified(bool value); // Emits verification result: True = verified
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

    // Helper Methods
    void resendOtpWithTimer();
    void disableControls(QString btnText);

    // Slots
    void onResendClicked();
    void onMaxLimitReached();
    void onVerifyClicked();
    void onCancelClicked();
    void onTimeout();
    void onSomethingWrong();
};
