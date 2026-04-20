#pragma once

#include <QSettings>
#include <QObject>

namespace Core::Services::Auth {
    class Signin;
    class GetOTP;
    class Signup;
};

namespace Ui::Auth {
    class AuthWindow;
    class Signin;
    class Signup;
    class Otp;
};

class AuthCoordinator : public QObject {
    Q_OBJECT

    public:
    AuthCoordinator(QObject *parent = nullptr);
    void show();
    
    private:
    // Settings
    QSettings settings;

    // Account Window
    Ui::Auth::AuthWindow *authWindow = nullptr;

    // Account Sign In
    Ui::Auth::Signin *accountSignIn = nullptr;

    // Account Create
    Ui::Auth::Signup *accountSignup = nullptr;

    // Account OTP
    Ui::Auth::Otp *accountOTP = nullptr;

    // Account Sign In Service
    Core::Services::Auth::Signin *signinService = nullptr;

    // Account Creation Service
    Core::Services::Auth::Signup *signupService = nullptr;

    // GetOTP
    Core::Services::Auth::GetOTP *getOTP = nullptr;

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