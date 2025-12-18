#include "accountCreationManager.h"

AccountCreationManager::AccountCreationManager(QObject *parent) : QObject(parent) {
    emailValidator = new GetEmail(this);
    usernameValidator = new GetUsername(this);
    pwdValidator = new GetPassword(this);
    nameValidator = new GetName(this); 
}

void AccountCreationManager::setAccountCreateObject(AccountCreate* accountCreateObj) {
   if (!accountCreateObj) return;
   this->accountCreate = accountCreateObj;

   emailValidator->setAccountCreateObject(this->accountCreate);
   usernameValidator->setAccountCreateObject(this->accountCreate);
   pwdValidator->setAccountCreateObject(this->accountCreate);
   nameValidator->setAccountCreateObject(this->accountCreate);   
   setupConnections();
}

void AccountCreationManager::setupConnections() {
    connect(usernameValidator, &GetUsername::usernameValidated, this, &AccountCreationManager::onUsernameValidated);
    connect(emailValidator, &GetEmail::emailValidated, this, &AccountCreationManager::onEmailValidated);
    connect(pwdValidator, &GetPassword::pwdValidated, this, &AccountCreationManager::onPwdValidated);
    connect(nameValidator, &GetName::nameValidated, this, &AccountCreationManager::onNameValidated);
    connect(this, &AccountCreationManager::validationDone, this, &AccountCreationManager::onValidationDone);
    connect(accountCreate->termsCondsBtn(), &CheckWithBtn::boxChecked, this, &AccountCreationManager::onTCBoxCheck);
}

void AccountCreationManager::checkValidationStatus() {
    for (const auto &it : validationStatus) {
        if (it.second == false) {
            emit validationDone(false);
            return;
        }
    }
    emit validationDone(true);
}

void AccountCreationManager::onUsernameValidated(bool isValid) {
    validationStatus["username"] = isValid;
    checkValidationStatus();
}

void AccountCreationManager::onEmailValidated(bool isValid) {
    validationStatus["email"] = isValid;
    checkValidationStatus();
}

void AccountCreationManager::onPwdValidated(bool isValid) {
    validationStatus["password"] = isValid;
    qDebug() << "Password validation result: " << (isValid ? "Valid" : "Invalid") << "\n";
    checkValidationStatus();
}

void AccountCreationManager::onValidationDone(bool isValidationDone) {
    if (!accountCreate) return;
    accountCreate->createBtn()->setEnabled(isValidationDone);
}

void AccountCreationManager::onTCBoxCheck(bool checked) {
    validationStatus["acceptedTC"] = checked;
    qDebug() << "Does the user accepted T&C: " << (checked ? "Accepted" : "Declined") << "\n";
    checkValidationStatus();
}

void AccountCreationManager::onNameValidated(bool isValid) {
    validationStatus["fullName"] = isValid;
    qDebug() << "Full Name validation result: " << (isValid ? "Valid" : "Invalid") << "\n";
    checkValidationStatus();
}
