#pragma once

#include "../../ui/accountCreate/AccountCreate.h"
#include "../accountCreationManager/AccountCreationManager.h"

#include "../../ui/accountOTP/AccountOTP.h"
#include "../validators/OTPValidator/OTPValidator.h"

#include "../../ui/accountWindow/AccountWindow.h"

#include "../../ui/accountSignIn/AccountSignIn.h"
#include "../accountSignInManager/AccountSignInManager.h"

#include <QSettings>
#include <QObject>
#include <memory>

class AccountAuthCoordinator {
    public:
    AccountAuthCoordinator();
    ~AccountAuthCoordinator();
    void show();

    private:
    QSettings settings;
    const QString key = "account/registered";

    void setAccountRegistered(bool isRegistered);
    bool isAccountRegistered() const;

    std::unique_ptr<AccountWindow> accountWindow;

    std::unique_ptr<AccountSignIn> accountSignIn;
    std::unique_ptr<AccountCreate> accountCreate;
    std::unique_ptr<AccountOTP> accountOTP;

    std::unique_ptr<AccountSignInManager> accountSignInManager;
    std::unique_ptr<AccountCreationManager> accountCreateManager;
    std::unique_ptr<GetOTP> getOTP;

    void showSignIn();
    void showCreateAccount();
    void showOTP(const QString &email, const QString &username, const QString &fullName);

    // Slots
    void onCredentialsStoredSuccessfully();
};