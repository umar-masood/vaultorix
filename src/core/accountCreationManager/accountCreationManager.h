#pragma once

#include "../validators/emailValidator/emailValidator.h"
#include "../validators/usernameValidator/usernameValidator.h"
#include "../validators/pwdValidator/pwdValidator.h"
#include "../validators/nameValidator/nameValidator.h"
#include "../deviceInfo/deviceInfo.h"

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

    const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";
    const QString API_URL = "https://www.umarcreations.site/store-credentials";

    QNetworkAccessManager *manager = nullptr;

    QString message;
    int statusCode;

    DeviceInfo deviceInfo;
    
    AccountCreate* accountCreate = nullptr;
    GetEmail* emailValidator = nullptr;
    GetUsername* usernameValidator = nullptr;
    GetPassword* pwdValidator = nullptr;
    GetName* nameValidator = nullptr;

    void setupConnections();
    void checkValidationStatus();
    void storeCredentials();

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