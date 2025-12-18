#pragma once

#include "../validators/emailValidator/emailValidator.h"
#include "../validators/usernameValidator/usernameValidator.h"
#include "../validators/pwdValidator/pwdValidator.h"
#include "../validators/nameValidator/nameValidator.h"

class AccountCreationManager : public QObject{
    Q_OBJECT

    public:
    explicit AccountCreationManager(QObject *parent = nullptr);

    void setAccountCreateObject(AccountCreate* accountCreateObj);

    private:
    std::unordered_map<std::string, bool> validationStatus = {
        {"username", false},
        {"email", false},
        {"password", false},
        {"fullName", false},
        {"acceptedTC", false}
    };

    AccountCreate* accountCreate = nullptr;
    GetEmail* emailValidator = nullptr;
    GetUsername* usernameValidator = nullptr;
    GetPassword* pwdValidator = nullptr;
    GetName* nameValidator = nullptr;

    void setupConnections();
    void checkValidationStatus();

    signals:
    void validationDone(bool isValidationDone);

    private slots:
    void onNameValidated(bool isValid);
    void onUsernameValidated(bool isValid);
    void onEmailValidated(bool isValid);
    void onPwdValidated(bool isValid);
    void onValidationDone(bool isValidationDone);
    void onTCBoxCheck(bool checked);
};