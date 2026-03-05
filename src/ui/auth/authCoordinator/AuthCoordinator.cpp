#include "AuthCoordinator.h"

/* ========================================================================================= 
                              ACCOUNT AUTH COORDINATOR              
   ========================================================================================= */
AuthCoordinator::AuthCoordinator(QObject *parent) : QObject(parent) {
    // Account Window
    authWindow = std::make_unique<AuthWindow>();

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
void AuthCoordinator::show() {
    if (authWindow)
        authWindow->show();
}

// Set account registered status in registry key
void AuthCoordinator::setAccountRegistered(bool isRegistered) {
    settings.setValue(key, isRegistered);
}  

// Display sign in page
void AuthCoordinator::showSignIn() {
    // If account sign in pointer already exists
    if (accountSignIn) {
        accountSignIn = nullptr;
        accountSignIn.reset();
    }

    // Create Account Sign In pointer
    accountSignIn = std::make_unique<Signin>();

    // Account Sign In Service pointer
    signinService = std::make_unique<SigninService>(authWindow.get());
    signinService->setAccountSignin(accountSignIn.get());

    // Signal Slot of Account Sign in service
    connect(signinService.get(), &SigninService::verificationNeeded, this, &AuthCoordinator::onVerificationNeeded);

    // If account window already exists
    if (authWindow) 
        authWindow->setRightWidget(accountSignIn.get());

    // Signal Slot of Account Sign in
    connect(accountSignIn->redirectToSignup(), &TextWithBtn::buttonClicked, this, &AuthCoordinator::showCreateAccount);
}

// Display OTP Verification page 
void AuthCoordinator::showOTP(const QString &email, const QString &username, const QString &fullName) {
    // If account OTP pointer already exists
    if (accountOTP) {
        accountOTP = nullptr;
        accountOTP.reset();
    }

    // Make new pointer of account otp
    accountOTP = std::make_unique<Otp>();

    // If get get OTP pointer already exists
    if (getOTP) {
        getOTP = nullptr;
        getOTP.reset();
    }

    // Make new get OTP pointer 
    getOTP = std::make_unique<GetOTP>(authWindow.get());
    getOTP->setAccountOtp(accountOTP.get(), email, username, fullName);

    // If account window exists
    if (authWindow) 
        authWindow->setRightWidget(accountOTP.get());
    
    // Signal Slot
    connect(getOTP.get(), &GetOTP::OTPVerified, this, &AuthCoordinator::onOTPVerified);
}

// Display Account Create page
void AuthCoordinator::showCreateAccount() {
    // If account create pointer already exists
    if (accountSignup) {
        accountSignup = nullptr;
        accountSignup.reset();
    }

    // Make new account create
    accountSignup = std::make_unique<Signup>(nullptr, authWindow.get()); // Here, we have passed authWindow pointer because inside Signup class we have used a dailog box for terms and conditions. To make it a child of parent (SubWindow)    
    authWindow->setRightWidget(accountSignup.get());

    // Signal Slot of redirecting to Sign In page from Sign Up page.
    connect(accountSignup->redirectToSignin(), &TextWithBtn::buttonClicked, this, &AuthCoordinator::showSignIn);

    // If account create manager already exists
    if (signupService) {
        signupService = nullptr;
        signupService.reset();
    }

    // Making new pointer of account create manager
    signupService = std::make_unique<SignupService>(authWindow.get()); // Similarly, authWindow is also passed here for dialogs parenting.
    signupService->setAccountSignup(accountSignup.get());

    // Connecting signal slot
    connect(signupService.get(), &SignupService::credentialsStoredSuccessfully, this, &AuthCoordinator::onCredentialsStoredSuccessfully);
}

/* -----------------  Getters --------------------- */
bool AuthCoordinator::isAccountRegistered() const {
    if (!settings.contains(key)) 
        return false;

    return settings.value(key).toBool();
}

/* ------------------- Slots -------------------------*/
void AuthCoordinator::onCredentialsStoredSuccessfully() {
    // Show OTP
    showOTP(accountSignup->emailField()->text(), accountSignup->usernameField()->text(), accountSignup->nameField()->text());
    setAccountRegistered(true);
}

// Verification needed slot (emit when user account verification is incomplete)
void AuthCoordinator::onVerificationNeeded(const QString &name, const QString &email, const QString &username) {
    // Show OTP page
    qDebug() << name << email << username;
    showOTP(email, username, name);
}

// When otp is verified then show Sign in page,
void AuthCoordinator::onOTPVerified(bool isVerified) {
    // If it is verified
    if (isVerified)
        // Show sign in 
        showSignIn();
}