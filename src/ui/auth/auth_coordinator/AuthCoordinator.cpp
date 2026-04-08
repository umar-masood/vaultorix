#include "AuthCoordinator.h"

#include "../../../core/config/Constants.h"

#include "../signup/Signup.h"
#include "../../../core/services/auth/SignupService.h"

#include "../otp/Otp.h"
#include "../../../core/services/auth/OTPService.h"

#include "../auth_window/AuthWindow.h"

#include "../signin/Signin.h"
#include "../../../core/services/auth/SigninService.h"

/* ========================================================================================= 
                              ACCOUNT AUTH COORDINATOR              
   ========================================================================================= */
AuthCoordinator::AuthCoordinator(QObject *parent) : QObject(parent) {
    // Account Window
    authWindow = new AuthWindow;

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
    if (accountSignIn) 
        accountSignIn = nullptr;

    // Create Account Sign In pointer
    accountSignIn = new Signin;

    // Account Sign In Service pointer
    signinService = new SigninService(authWindow, this);
    signinService->setAccountSignin(accountSignIn);

    // Signal Slot of Account Sign in service
    connect(signinService, &SigninService::verificationNeeded, this, &AuthCoordinator::onVerificationNeeded);

    // If account window already exists
    if (authWindow) 
        authWindow->setRightWidget(accountSignIn);

    // Signal Slot of Account Sign in
    connect(accountSignIn->redirectToSignup(), &TextWithBtn::buttonClicked, this, &AuthCoordinator::showCreateAccount);
}

// Display OTP Verification page 
void AuthCoordinator::showOTP(const QString &email) {
    // If account OTP pointer already exists
    if (accountOTP) 
        accountOTP = nullptr;    

    // Make new pointer of account otp
    accountOTP = new Otp;

    // If get get OTP pointer already exists
    if (getOTP) 
        getOTP = nullptr;

    // Make new get OTP pointer 
    getOTP = new GetOTP(authWindow, this);
    getOTP->setAccountOtp(accountOTP, email);

    // If account window exists
    if (authWindow) 
        authWindow->setRightWidget(accountOTP);
    
    // Signal Slot
    connect(getOTP, &GetOTP::OTPVerified, this, &AuthCoordinator::onOTPVerified);
}

// Display Account Create page
void AuthCoordinator::showCreateAccount() {
    // If account create pointer already exists
    if (accountSignup) 
        accountSignup = nullptr;

    // Make new account create
    accountSignup = new Signup(nullptr, authWindow); // Here, we have passed authWindow pointer because inside Signup class we have used a dailog box for terms and conditions. To make it a child of parent (SubWindow)    
    authWindow->setRightWidget(accountSignup);

    // Signal Slot of redirecting to Sign In page from Sign Up page.
    connect(accountSignup->redirectToSignin(), &TextWithBtn::buttonClicked, this, &AuthCoordinator::showSignIn);

    // If account create manager already exists
    if (signupService) 
        signupService = nullptr;

    // Making new pointer of account create manager
    signupService = new SignupService(authWindow, this); // Similarly, authWindow is also passed here for dialogs parenting.
    signupService->setAccountSignup(accountSignup);

    // Connecting signal slot
    connect(signupService, &SignupService::credentialsStoredSuccessfully, this, &AuthCoordinator::onCredentialsStoredSuccessfully);
}

/* -----------------  Getters --------------------- */
bool AuthCoordinator::isAccountRegistered() const {
    if (!settings.contains(ACCOUNT_REGISTERED_SETTINGS_KEY)) 
        return false;

    return settings.value(ACCOUNT_REGISTERED_SETTINGS_KEY).toBool();
}

/* ------------------- Slots -------------------------*/
void AuthCoordinator::onCredentialsStoredSuccessfully() {
    // Show OTP
    showOTP(accountSignup->emailField()->text());

    // Updating status in registry
    if (!isAccountRegistered())
        setAccountRegistered(true);
}

// Verification needed slot (emit when user account verification is incomplete)
void AuthCoordinator::onVerificationNeeded(const QString &email) {
    // Show OTP page
    showOTP(email);
}

// When otp is verified then show Sign in page,
void AuthCoordinator::onOTPVerified(bool isVerified) {
    // If it is verified
    if (isVerified)
        // Show sign in 
        showSignIn();
}