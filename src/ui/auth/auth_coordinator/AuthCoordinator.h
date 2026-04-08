#pragma once

#include <QSettings>
#include <QObject>

class AuthWindow;
class Signin;
class Signup;
class GetOTP;
class SigninService;
class SignupService;
class Otp;

class AuthCoordinator : public QObject {
    Q_OBJECT

    public:
    AuthCoordinator(QObject *parent = nullptr);
    void show();
    
    private:
    // Settings
    QSettings settings;

    // Account Window
    AuthWindow *authWindow = nullptr;

    // Account Sign In
    Signin *accountSignIn = nullptr;

    // Account Create
    Signup *accountSignup = nullptr;

    // Account OTP
    Otp *accountOTP = nullptr;

    // Account Sign In Service
    SigninService *signinService = nullptr;

    // Account Creation Service
    SignupService *signupService = nullptr;

    // GetOTP
    GetOTP *getOTP = nullptr;

    void setAccountRegistered(bool isRegistered);
    bool isAccountRegistered() const;

    void showOTP(const QString &email);
    void showSignIn();
    void showCreateAccount();

    // Slots
    void onCredentialsStoredSuccessfully();
    void onVerificationNeeded(const QString &email);
    void onOTPVerified(bool isVerified);
};