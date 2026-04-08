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
#include <expected>

#include "../../../ui/auth/auth_window/AuthWindow.h"
#include "../../../ui/auth/otp/Otp.h"
#include "../../../ui/dialogs/error_dialog/ErrorDialog.h"

/* ------------------ OTP Validator ------------- */
class OTPService : public QObject {
    Q_OBJECT

    public:
    // OTP Verify Error Struct
    struct OTPError {
        int status_code;
        QString message;
    };

    OTPService(QObject *parent = nullptr);
    int sendOTP(const QString &email);
    std::expected<bool, OTPError> verifyOTP(const QString &otp, const QString &email);

    private:
    // Network Manager
    QNetworkAccessManager *manager = nullptr;

    // API Credentials
    const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";
    
    // API Response
    int status_code;
    QString message;    

    // Verification flag for OTP
    bool isVerified = false;

    // Helpers
    QNetworkRequest configureNetworkRequest(const QString &route);
};

/* ----------------------  Get OTP ------------------------ */
class GetOTP : public QObject {
    Q_OBJECT

    public:
    explicit GetOTP(AuthWindow *ac = nullptr, QObject *parent = nullptr);
    bool setAccountOtp(Otp *instance, const QString &email);

    signals:
    void OTPVerified(bool isVerified); // Emits verification result: True = verified
    void maxLimitReached();  
    void somethingWrong();      

    private:
    // Current Account OTP 
    Otp *ao = nullptr;

    // OTP Validator
    OTPService *ov = nullptr;

    // Stores email, current otp from textfields
    QString cEmail, currOtp;

    // OTP Expiry
    int totalSecs = 90;
    QTimer *timer = nullptr;
    QString timeString;

    // Error Dialog
    ErrorDialogManager *errorManager = nullptr;
    
    // Helper Methods
    void resendOtpWithTimer();
    void disableControls(QString btnText);
    bool handleSentOTPResponse();

    // Slots
    void onResendClicked();
    void onMaxLimitReached();
    void onVerifyClicked();
    void onCancelClicked() const;
    void onTimeout();
    void onSomethingWrong();
};
