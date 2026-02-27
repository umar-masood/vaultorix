#pragma once

#include "../../../ui/auth/signup/Signup.h"
#include "../../services/auth/SignupService.h"

#include "../../../ui/auth/otp/Otp.h"
#include "../../validators/auth/OTPValidator.h"
#include "../../../ui/auth/authWindow/AuthWindow.h"

#include "../../../ui/auth/signin/Signin.h"
#include "../../services/auth/SigninService.h"

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