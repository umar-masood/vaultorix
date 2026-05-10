#include "AuthCoordinator.h"

#include "../../../core/config/Constants.h"

#include "../signin/Signin.h"
#include "../signup/Signup.h"
#include "../otp/OTP.h"
#include "../auth_window/AuthWindow.h"
#include "../../utils/Utils.h"

/* ========================================================================================= 
                              ACCOUNT AUTH COORDINATOR              
   ========================================================================================= */
AuthCoordinator::AuthCoordinator(QObject *parent) : QObject(parent) {
    // Account Window
    authWindow = new  Ui::AuthWindow;

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
    settings.setValue(ACCOUNT_REGISTERED_SETTINGS_KEY, isRegistered);
}  

// Display sign in page
void AuthCoordinator::showSignIn() {
    // If account sign in pointer already exists
    if (accountSigninWidget) 
        accountSigninWidget = nullptr;

    // Create Account Sign In pointer
    accountSigninWidget = new Ui::Signin(nullptr, authWindow);

    // If account window already exists
    if (authWindow) 
        authWindow->setRightWidget(accountSigninWidget);

    // Signal Slots of Account Sign in
    connect(accountSigninWidget, &Ui::Signin::verificationRequired, this, &AuthCoordinator::onVerificationRequired);
    connect(accountSigninWidget->redirectToSignUpWidget(), &Ui::Utils::TextWithBtn::buttonClicked, this, &AuthCoordinator::showCreateAccount);
}

// Display OTP Verification page 
void AuthCoordinator::showOTP(const QString &email, const QString &authType) {
    // If account OTP pointer already exists
    if (accountOTPWidget) 
        accountOTPWidget = nullptr;    

    // Make new pointer of account otp
    accountOTPWidget = new Ui::OTP(nullptr, authWindow);
    accountOTPWidget->setEmail(email);
    accountOTPWidget->setAuthType(authType);

    // If account window exists
    if (authWindow) 
        authWindow->setRightWidget(accountOTPWidget);
    
    // Signal Slot
    // OTPVerified signal only emits in case if OTP is required after Sign Up
    connect(accountOTPWidget, &Ui::OTP::OTPVerified, this, &AuthCoordinator::onOTPVerified);
}

// Display Account Create page
void AuthCoordinator::showCreateAccount() {
    // If account create pointer already exists
    if (accountSignupWidget) 
        accountSignupWidget = nullptr;

    // Make new account create
    accountSignupWidget = new Ui::Signup(nullptr, authWindow); 
    authWindow->setRightWidget(accountSignupWidget);

    // Signal Slot of redirecting to Sign In page from Sign Up page.
    connect(accountSignupWidget->redirectToSignin(), &Ui::Utils::TextWithBtn::buttonClicked, this, &AuthCoordinator::showSignIn);

    // Connecting signal slot
    connect(accountSignupWidget, &Ui::Signup::credentialsStored, this, &AuthCoordinator::onCredentialsStored);
}

/* -----------------  Getters --------------------- */
bool AuthCoordinator::isAccountRegistered() const {
    if (!settings.contains(ACCOUNT_REGISTERED_SETTINGS_KEY)) 
        return false;

    return settings.value(ACCOUNT_REGISTERED_SETTINGS_KEY).toBool();
}

/* ------------------- Slots -------------------------*/
void AuthCoordinator::onCredentialsStored(const QString &email) {
    // Show OTP
    showOTP(email);

    // Updating status in registry
    if (!isAccountRegistered())
        setAccountRegistered(true);
}

// Verification needed slot
void AuthCoordinator::onVerificationRequired(const QString &email, const QString &authType) {
    // Show OTP 
    showOTP(email, authType);
}

// When otp is verified then show Sign in page,
void AuthCoordinator::onOTPVerified(bool isVerified) {
    // If it is verified
    if (isVerified)
        // Show sign in 
        showSignIn();
}