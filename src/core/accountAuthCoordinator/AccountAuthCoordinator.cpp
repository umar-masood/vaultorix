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

void AccountAuthCoordinator::show() {
    if (accountWindow)
        accountWindow->show();
}

bool AccountAuthCoordinator::isAccountRegistered() const {
    if (!settings.contains(key)) 
        return false;

    return settings.value(key).toBool();
}

void AccountAuthCoordinator::setAccountRegistered(bool isRegistered) {
    settings.setValue(key, isRegistered);
}  

void AccountAuthCoordinator::showSignIn() {
    if (accountSignIn) {
        accountSignIn = nullptr;
        accountSignIn->setParent(nullptr);
    }

    accountSignIn = std::make_unique<AccountSignIn>();
    
    if (accountWindow) 
        accountWindow->setRightWidget(accountSignIn.get());
}

void AccountAuthCoordinator::showOTP(const QString &email, const QString &username, const QString &fullName) {
    if (accountOTP) {
        accountOTP = nullptr;
        accountOTP->setParent(nullptr);
    }

    accountOTP = std::make_unique<AccountOTP>();

    if (getOTP) {
        getOTP = nullptr;
        getOTP->setParent(nullptr);
    }

    getOTP = std::make_unique<GetOTP>();
    getOTP->setAccountOTPObjectWithDetails(accountOTP.get(), email, username, fullName);

    if (accountWindow) 
        accountWindow->setRightWidget(accountOTP.get());
}

void AccountAuthCoordinator::showCreateAccount() {
    if (accountCreate) {
        accountCreate = nullptr;
        accountCreate->setParent(nullptr);
    }

    accountCreate = std::make_unique<AccountCreate>(nullptr, accountWindow.get()); // Here, we have passed accountWindow pointer because inside AccountCreate class we have used a dailog box for terms and conditions. To make it a child of parent (SubWindow)    
    accountWindow->setRightWidget(accountCreate.get());

    if (accountCreateManager) {
        accountCreateManager = nullptr;
        accountCreateManager->setParent(nullptr);
    }

    accountCreateManager = std::make_unique<AccountCreationManager>(accountWindow.get()); // Similarly, accountWindow is also passed here for dialogs parenting.
    accountCreateManager->setAccountCreateObject(accountCreate.get());

    // Connecting signal slot
    QObject::connect(accountCreateManager.get(), &AccountCreationManager::credentialsStoredSuccessfully, [this]() { onCredentialsStoredSuccessfully(); });
}

void AccountAuthCoordinator::onCredentialsStoredSuccessfully() {
    showOTP(accountCreate->emailField()->text(), accountCreate->usernameField()->text(), accountCreate->nameField()->text());
    setAccountRegistered(true);
}