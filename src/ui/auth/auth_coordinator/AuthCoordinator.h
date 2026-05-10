#pragma once

#include <QSettings>
#include <QObject>

namespace Ui {
    class AuthWindow;
    class Signin;
    class Signup;
    class OTP;
};

class AuthCoordinator : public QObject {
    Q_OBJECT

    public:
    explicit AuthCoordinator(QObject *parent = nullptr);
    void show();
    
    private:
    // Settings
    QSettings settings;

    // Account Window
    Ui::AuthWindow *authWindow = nullptr;

    // Account Sign In Widget
    Ui::Signin *accountSigninWidget = nullptr;

    // Account Sign Up Widget
    Ui::Signup *accountSignupWidget = nullptr;

    // Account OTP Widget
    Ui::OTP *accountOTPWidget = nullptr;

    void setAccountRegistered(bool isRegistered);
    bool isAccountRegistered() const;

    void showOTP(const QString &email, const QString &authType = QString());
    void showSignIn();
    void showCreateAccount();

    private slots:
    void onCredentialsStored(const QString &email);
    void onVerificationRequired(const QString &email, const QString &authType = QString());
    void onOTPVerified(bool isVerified);
};