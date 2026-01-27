#include "AccountAuthCoordinator.h"

/* ========================================================================================= 
                              ACCOUNT AUTH COORDINATOR              
   ========================================================================================= */
AccountAuthCoordinator::AccountAuthCoordinator(QObject *parent) {
    // Account Window
    accountWindow = std::make_unique<AccountWindow>();

    // Checking weather the user has already registered its account once
    if (!isAccountRegistered())
        // Show create account page
        showCreateAccount();
    else
        // Show sign in page
        showSignIn();
}

/* ------------------ Setters -------------------- */
// Show main window
void AccountAuthCoordinator::show() {
    if (accountWindow)
        accountWindow->show();
}

// Set account registered status in registry key
void AccountAuthCoordinator::setAccountRegistered(bool isRegistered) {
    settings.setValue(key, isRegistered);
}  

// Display sign in page
void AccountAuthCoordinator::showSignIn() {
    // If account sign in pointer already exists
    if (accountSignIn) {
        accountSignIn = nullptr;
        accountSignIn.reset();
    }

    // Create Account Sign In pointer
    accountSignIn = std::make_unique<AccountSignIn>();

    // Account Sign In Manager pointer
    accountSignInManager = std::make_unique<AccountSignInManager>(accountWindow.get());
    accountSignInManager->setAccountSignInObject(accountSignIn.get());

    // Signal Slot of Account Sign in manager
    connect(accountSignInManager.get(), &AccountSignInManager::verificationNeeded, this, &AccountAuthCoordinator::onVerificationNeeded);

    // If account window already exists
    if (accountWindow) 
        accountWindow->setRightWidget(accountSignIn.get());

    // Signal Slot of Account Sign in
    connect(accountSignIn->redirectToSignUpWidget(), &TextWithBtn::buttonClicked, this, &AccountAuthCoordinator::showCreateAccount);
}

// Display OTP Verification page 
void AccountAuthCoordinator::showOTP(const QString &email, const QString &username, const QString &fullName) {
    // If account OTP pointer already exists
    if (accountOTP) {
        accountOTP = nullptr;
        accountOTP.reset();
    }

    // Make new pointer of account otp
    accountOTP = std::make_unique<AccountOTP>();

    // If get get OTP pointer already exists
    if (getOTP) {
        getOTP = nullptr;
        getOTP.reset();
    }

    // Make new get OTP pointer 
    getOTP = std::make_unique<GetOTP>(accountWindow.get());
    getOTP->setAccountOTPObjectWithDetails(accountOTP.get(), email, username, fullName);

    // If account window exists
    if (accountWindow) 
        accountWindow->setRightWidget(accountOTP.get());
    
    // Signal Slot
    connect(getOTP.get(), &GetOTP::OTPVerified, this, &AccountAuthCoordinator::onOTPVerified);
}

// Display Account Create page
void AccountAuthCoordinator::showCreateAccount() {
    // If account create pointer already exists
    if (accountCreate) {
        accountCreate = nullptr;
        accountCreate.reset();
    }

    // Make new account create
    accountCreate = std::make_unique<AccountCreate>(nullptr, accountWindow.get()); // Here, we have passed accountWindow pointer because inside AccountCreate class we have used a dailog box for terms and conditions. To make it a child of parent (SubWindow)    
    accountWindow->setRightWidget(accountCreate.get());

    // Signal Slot of redirecting to Sign In page from Sign Up page.
    connect(accountCreate->redirectToSignInWidget(), &TextWithBtn::buttonClicked, this, &AccountAuthCoordinator::showSignIn);

    // If account create manager already exists
    if (accountCreateManager) {
        accountCreateManager = nullptr;
        accountCreateManager.reset();
    }

    // Making new pointer of account create manager
    accountCreateManager = std::make_unique<AccountCreateManager>(accountWindow.get()); // Similarly, accountWindow is also passed here for dialogs parenting.
    accountCreateManager->setAccountCreateObject(accountCreate.get());

    // Connecting signal slot
    connect(accountCreateManager.get(), &AccountCreateManager::credentialsStoredSuccessfully, this, [this]() { onCredentialsStoredSuccessfully(); });
}

/* -----------------  Getters --------------------- */
bool AccountAuthCoordinator::isAccountRegistered() const {
    if (!settings.contains(key)) 
        return false;

    return settings.value(key).toBool();
}

/* ------------------- Slots -------------------------*/
void AccountAuthCoordinator::onCredentialsStoredSuccessfully() {
    // Show OTP
    showOTP(accountCreate->emailField()->text(), accountCreate->usernameField()->text(), accountCreate->nameField()->text());
    setAccountRegistered(true);
}

// Verification needed slot (emit when user account verification is incomplete)
void AccountAuthCoordinator::onVerificationNeeded(const QString &name, const QString &email, const QString &username) {
    // Show OTP page
    qDebug() << name << email << username;
    showOTP(email, username, name);
}

// When otp is verified then show Sign in page,
void AccountAuthCoordinator::onOTPVerified(bool isVerified) {
    // If it is verified
    if (isVerified)
        // Show sign in 
        showSignIn();
}
