#pragma once

#include "../validators/emailValidator/emailValidator.h"
#include "../validators/usernameValidator/usernameValidator.h"
#include "../validators/pwdValidator/pwdValidator.h"
#include "../validators/nameValidator/nameValidator.h"
#include "../../ui/dialogs/errorDialog/errorDialog.h"
#include "../deviceInfo/deviceInfo.h"

#include <QSettings>

class AccountCreationManager : public QObject{
    Q_OBJECT

    public:
    explicit AccountCreationManager(AccountWindow *accountWindow = nullptr, QObject *parent = nullptr);
    void setAccountCreateObject(AccountCreate* accountCreateObj);

    private:
    std::unordered_map<std::string, bool> validationStatus = {
        {"username", false},
        {"email", false},
        {"password", false},
        {"fullName", false},
        {"acceptedTC", false}
    };

    QNetworkAccessManager *manager = nullptr;
    const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";
    const QString API_URL = "https://www.umarcreations.site/store-credentials";
    QString message;
    int statusCode;

    DeviceInfo deviceInfo;
    QSettings *settings = nullptr;

    ErrorDialogManager *errorDialogManager = nullptr;
    
    AccountWindow *accountWindow = nullptr;
    AccountCreate* accountCreate = nullptr;

    GetEmail* emailValidator = nullptr;
    GetUsername* usernameValidator = nullptr;
    GetPassword* pwdValidator = nullptr;
    GetName* nameValidator = nullptr;

    void setupConnections();
    void checkValidationStatus();

    QJsonObject getCredentials();
    void storeCredentials();

    void storeDeviceId();
    QString getDeviceIdLocally() const;
    
    void updateCreateAccBtnState(bool isEnabled, const QString &text);
    void handleCreateAccError(const QString &errorName, bool createAccButtonEnabled = false, const QString &createAccButtonText = "Create Account");

    signals:
    void validationDone(bool isValidationDone);
    void credentialsStoredSuccessfully();

    private slots:
    void onNameValidated(bool isValid);
    void onUsernameValidated(bool isValid);
    void onEmailValidated(bool isValid);
    void onPwdValidated(bool isValid);
    void onValidationDone(bool isValidationDone);
    void onTCBoxCheck(bool checked);
    void onCreateAccBtnClicked();
    void onErrorDialogActionBtnClicked(const QString &key);
    void onCredentialsStoredSuccessfully();
};