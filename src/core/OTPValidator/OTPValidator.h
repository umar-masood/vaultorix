#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QNetworkAccessManager>
#include "ui/accountOTP/AccountOTP.h"

class OTPValidator {
public:
    OTPValidator();

    int sendOTP(const QString &fullName, const QString &username, const QString &email);
    bool verifyOTP(const QString &otp, const QString &email);

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
    explicit GetOTP(QObject *parent = nullptr);

    bool setAccountOTPObjectWithDetails(AccountOTP *ao, QString &email, QString &username, QString &name);

signals:
    void otpVerified(bool value); // Emits verification result: True = verified
    void maxLimitReached();       // Emits when resend limit (HTTP 429) reached

private slots:
    void onResendClicked();
    void onMaxLimitReached();
    void onVerifyClicked();
    void onCancelClicked();
    void onTimeout();

private:
    AccountOTP *ao = nullptr;
    OTPValidator *ov = nullptr;

    QString cEmail, cUserName, cName;
    QString currOtp;

    int totalSecs = 90;
    QTimer *timer = nullptr;
    QString timeString;

    void resendOtpWithTimer();
};
