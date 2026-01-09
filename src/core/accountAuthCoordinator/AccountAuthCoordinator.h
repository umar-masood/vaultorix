#pragma once

#include "../../ui/accountCreate/AccountCreate.h"
#include "../accountCreationManager/AccountCreationManager.h"

#include "../../ui/accountOTP/AccountOTP.h"
#include "../../ui/accountWindow/AccountWindow.h"

#include "../../ui/accountSignIn/AccountSignIn.h"
#include "../accountSignInManager/AccountSignInManager.h"

#include <QSettings>
#include <memory>

class AccountAuthCoordinator {
    public:
    AccountAuthCoordinator();
    ~AccountAuthCoordinator();

    private:
    QSettings settings;
    const QString key = "account/registered";

    void setAccountRegistered(bool isRegistered);
    bool isAccountRegistered() const;

    std::unique_ptr<AccountWindow> accountWindow;

    std::unique_ptr<AccountSignIn> accountSignIn;
    std::unique_ptr<AccountCreate> accountCreate;

    std::unique_ptr<AccountSignInManager> accountSignInManager;
    std::unique_ptr<AccountCreationManager> accountCreateManager;

    void showSignIn();
    void showCreateAccount();
};