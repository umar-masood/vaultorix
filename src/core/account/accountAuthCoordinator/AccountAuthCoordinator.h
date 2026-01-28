#pragma once

#include "../../../ui/account/accountCreate/AccountCreate.h"
#include "../accountCreateManager/AccountCreateManager.h"

#include "../../../ui/account/accountOTP/AccountOTP.h"
#include "../validators/OTPValidator/OTPValidator.h"
#include "../../../ui/account/accountWindow/AccountWindow.h"

#include "../../../ui/account/accountSignIn/AccountSignIn.h"
#include "../accountSignInManager/AccountSignInManager.h"

#include <QSettings>
#include <QObject>
#include <memory>

class AccountAuthCoordinator : public QObject {
    Q_OBJECT

    public:
    AccountAuthCoordinator(QObject *parent = nullptr);
    void show();
    
    private:
    // Settings
    QSettings settings;
    const QString key = "account/registered";

    // Account Window
    std::unique_ptr<AccountWindow> accountWindow;

    // Account Sign In
    std::unique_ptr<AccountSignIn> accountSignIn;

    // Account Create
    std::unique_ptr<AccountCreate> accountCreate;

    // Account OTP
    std::unique_ptr<AccountOTP> accountOTP;

    // Account Sign In Manager
    std::unique_ptr<AccountSignInManager> accountSignInManager;

    // Account Creation Manager
    std::unique_ptr<AccountCreateManager> accountCreateManager;

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