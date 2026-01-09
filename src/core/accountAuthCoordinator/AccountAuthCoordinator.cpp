#include "AccountAuthCoordinator.h"

/* ---------- Account Auth Coordinator ----------- */
AccountAuthCoordinator::AccountAuthCoordinator() {
    // Account Window
    accountWindow = std::make_unique<AccountWindow>();

    // Checking weather the user has already registered its account once
    if (!isAccountRegistered())
        showCreateAccount();
    else
        showSignIn();
}

bool AccountAuthCoordinator::isAccountRegistered() const {
    if (!settings.contains(key)) 
        return false;

    return settings.value(key).toBool();
}

void AccountAuthCoordinator::setAccountRegistered(bool isRegistered) {
    if (!settings.contains(key)) 
        settings.setValue(key, isRegistered);
}  

void AccountAuthCoordinator::showSignIn() {
    if (accountSignIn) {
        accountSignIn = nullptr;
        accountSignIn->setParent(nullptr);
    }

    accountSignIn = std::make_unique<AccountSignIn>();
}

void AccountAuthCoordinator::showCreateAccount() {
    if (accountCreate) {
        accountCreate = nullptr;
        accountCreate->setParent(nullptr);
    }

    accountCreate = std::make_unique<AccountCreate>(nullptr, accountWindow.get()); // Here, we have passed accountWindow pointer becuase inside AccountCreate class we have used a dailog box for terms and conditions. To make it a child of parent (SubWindow)
    
    accountWindow->setRightWidget(accountCreate.get());

    if (accountCreateManager) {
        accountCreateManager = nullptr;
        accountCreateManager->setParent(nullptr);
    }

    accountCreateManager = std::make_unique<AccountCreationManager>(accountWindow.get()); // Similarly, accountWindow is also passed here for dialogs parenting.
    accountCreateManager->setAccountCreateObject(accountCreate.get());

}