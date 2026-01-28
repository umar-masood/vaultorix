#pragma once

#include "../validators/emailValidator/EmailValidator.h"
#include "../validators/usernameValidator/UsernameValidator.h"
#include "../validators/pwdValidator/PwdValidator.h"
#include "../validators/nameValidator/NameValidator.h"
#include "../../../ui/dialogs/errorDialog/ErrorDialog.h"
#include "../deviceInfo/DeviceInfo.h"

#include <QSettings>

class AccountCreateManager : public QObject {
    Q_OBJECT

    public:
    explicit AccountCreateManager(AccountWindow *accountWindow = nullptr, QObject *parent = nullptr);
    void setAccountCreateObject(AccountCreate* accountCreateObj);

    private:
    // Hashmap for Validation Status Check of all detail fields
    std::unordered_map<std::string, bool> validationStatus = {
        {"username"    , false},
        {"email"       , false},
        {"password"    , false},
        {"fullName"    , false},
        {"acceptedTC"  , false}
    };

    // Network Manager
    QNetworkAccessManager *manager = nullptr;

    // API Credentials
    const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";
    const QString API_URL = "https://www.umarcreations.site/store-credentials";

    // API Response
    QString message;
    int statusCode;

    // Device Information
    DeviceInfo deviceInfo;

    // Storing settings
    QSettings *settings = nullptr;

    // Error Dialog Manager
    ErrorDialogManager *errorDialogManager = nullptr;
    
    // Current Account Window
    AccountWindow *accountWindow = nullptr;
    AccountCreate* accountCreate = nullptr;

    // Validators 
    GetEmail* emailValidator = nullptr;
    GetUsername* usernameValidator = nullptr;
    GetPassword* pwdValidator = nullptr;
    GetName* nameValidator = nullptr;

    // Helper Methods
    void setupConnections();
    void checkValidationStatus();

    QJsonObject getCredentials();
    void storeCredentials();

    void storeDeviceId();
    QString getDeviceIdLocally() const;
    
    void updateCreateAccBtnState(bool isEnabled, const QString &text);
    void handleCreateAccError(const QString &errorName, bool createAccButtonEnabled = false, const QString &createAccButtonText = "Create Account");

    // Slots
    void onNameValidated(bool isValid);
    void onUsernameValidated(bool isValid);
    void onEmailValidated(bool isValid);
    void onPwdValidated(bool isValid);
    void onValidationDone(bool isValidationDone);
    void onTCBoxCheck(bool checked);
    void onCreateAccBtnClicked();
    void onErrorDialogActionBtnClicked(const QString &key);

    signals:
    void validationDone(bool isValidationDone);
    void credentialsStoredSuccessfully();
};