#pragma once

#include "../signup/Signup.h"
#include "../../../core/services/auth/SignupService.h"

#include "../otp/Otp.h"
#include "../../../core/validators/auth/OTPValidator.h"

#include "../authWindow/AuthWindow.h"

#include "../signin/Signin.h"
#include "../../../core/services/auth/SigninService.h"

#include <QSettings>
#include <QObject>
#include <memory>

class AuthCoordinator : public QObject {
    Q_OBJECT

    public:
    AuthCoordinator(QObject *parent = nullptr);
    void show();
    
    private:
    // Settings
    QSettings settings;
    const QString key = "account/registered";

    // Account Window
    std::unique_ptr<AuthWindow> authWindow;

    // Account Sign In
    std::unique_ptr<Signin> accountSignIn;

    // Account Create
    std::unique_ptr<Signup> accountSignup;

    // Account OTP
    std::unique_ptr<Otp> accountOTP;

    // Account Sign In Service
    std::unique_ptr<SigninService> signinService;

    // Account Creation Service
    std::unique_ptr<SignupService> signupService;

    // GetOTP
    std::unique_ptr<GetOTP> getOTP;

    void setAccountRegistered(bool isRegistered);
    bool isAccountRegistered() const;

    void showOTP(const QString &email, const QString &username, const QString &fullName);
    void showSignIn();
    void showCreateAccount();

    // Slots
    void onCredentialsStoredSuccessfully();
    void onVerificationNeeded(const QString &name, const QString &email, const QString &username);
    void onOTPVerified(bool isVerified);
};