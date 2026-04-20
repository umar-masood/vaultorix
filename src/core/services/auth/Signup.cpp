#include "Signup.h"

#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../utils/Utils.h"
#include "../../../ui/dialogs/error_dialog/ErrorDialog.h"
#include "../../../ui/auth/signup/Signup.h"
#include <QDebug>

using Core::Services::Auth::Signup;
/* ========================================================================================= 
                              ACCOUNT CREATE MANAGER            
   ========================================================================================= */
Signup::Signup(Ui::Auth::AuthWindow *authWindow, QObject *parent) : QObject(parent), authWindow(authWindow) {
    // Validators
    emailValidator = new GetEmail(this);
    usernameValidator = new GetUsername(this);
    passwordValidator = new GetPassword(this);
    nameValidator = new GetName(this);

    // Network Manager
    manager = new QNetworkAccessManager(this);

    // Settings
    settings = new QSettings(this);

    // Generate device id and stores it if it does not exists
    storeDeviceId();

    // Error Dialogs Manager
    errorDialogManager = ErrorDialogManager::instance();
}

void Signup::setAccountSignupWidget(Ui::Auth::Signup *instance) {
    // If account signup pointer does not initialized
    if (!instance) 
        return;
    
    // Initializing account signup poimter
    accountSignupWidget = instance;
    
    // Giving account create pointer to these member functions
    emailValidator->setAccountSignupWidget(accountSignupWidget);
    usernameValidator->setAccountSignupWidget(accountSignupWidget);
    passwordValidator->setAccountSignupWidget(accountSignupWidget);
    nameValidator->setAccountSignupWidget(accountSignupWidget);

    // Signal Slot Connections
    setupConnections();
}

void Signup::setupConnections() {
    connect(usernameValidator, &GetUsername::usernameValidated, this, &Signup::onUsernameValidated);
    connect(emailValidator, &GetEmail::emailValidated, this, &Signup::onEmailValidated);
    connect(passwordValidator, &GetPassword::pwdValidated, this, &Signup::onPwdValidated);
    connect(nameValidator, &GetName::nameValidated, this, &Signup::onNameValidated);
    connect(this, &Signup::validationDone, this, &Signup::onValidationDone);
    connect(accountSignupWidget->termsConditionsWidget(), &CheckWithBtn::boxChecked, this, &Signup::onTCBoxCheck);
    connect(accountSignupWidget->createAccountButton(), &Button::clicked, this, &Signup::onCreateAccBtnClicked);
    connect(errorDialogManager, &ErrorDialogManager::actionTriggered, this, &Signup::onErrorDialogActionBtnClicked);
}

// Check validation status for enable/disable create account button
void Signup::checkValidationStatus() {
    for (auto it = validationStatus.begin(); it != validationStatus.end(); ++it) {
        if (!it->second) {
            emit validationDone(false);
            return;
        }
    }
    emit validationDone(true);
}

// Preparing json data for request
QJsonObject Signup::getCredentials() {
    QJsonObject userObj;
    userObj["full_name"] = accountSignupWidget->nameField()->text();
    userObj["username"] = accountSignupWidget->usernameField()->text();
    userObj["email_address"] = accountSignupWidget->emailField()->text();
    userObj["password"] = accountSignupWidget->passwordField()->text();
    userObj["created_at"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    userObj["is_active"] = true;

    QJsonObject deviceObj;
    deviceObj["device_name"] = deviceInfo.getDeviceName();
    deviceObj["device_id"] = getDeviceIdLocally();    
    deviceObj["os_type"] = deviceInfo.getOSName();
    deviceObj["os_version"] = deviceInfo.getProductVersion();
    deviceObj["kernel_version"] = deviceInfo.getKernelVersion();

    QJsonObject appObj;
    appObj["version"] = QCoreApplication::applicationVersion();

    QJsonObject mainObj;
    mainObj["user_info"] = userObj;
    mainObj["device_info"] = deviceObj;
    mainObj["app_info"] = appObj;

    return mainObj;
}

// Calling api for storing user credentials
void Signup::storeCredentials() {
    QNetworkRequest request(QUrl(route(APIRoutes::SIGNUP)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("api_key", API_KEY.toUtf8());
    request.setTransferTimeout(REQUEST_TIMEOUT);

    QNetworkReply *reply = manager->post(request, QJsonDocument(getCredentials()).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::TimeoutError) {
            reply->deleteLater();
            handleCreateAccError("RequestTimeout", true);
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            qDebug() << reply->errorString();
            handleCreateAccError("SomethingWentWrong", true);
            return;
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
        reply->deleteLater();

        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            handleCreateAccError("SomethingWentWrong", true);
            return;
        }

        QJsonObject obj = doc.object();
        statusCode = obj["status_code"].toInt();
        message = obj["message"].toString();
        qDebug() << statusCode << message;
        if (statusCode == 200) 
            emit credentialsStoredSuccessfully(); 
        else if (statusCode == 403) 
            handleCreateAccError("MaxAttempts");
        else /*if (statusCode == 513) 
            handleCreateAccError("FurtherAttemptBlocked");
        else*/  // We will handle this later if we think it's necessary 
            handleCreateAccError("SomethingWentWrong", true);
    });
}

// Handling error dialogs and create account button status update
void Signup::handleCreateAccError(const QString &errorName, bool createAccButtonEnabled, const QString &createAccButtonText) {
    errorDialogManager->show(errorName, "Auth"); 
    updateCreateAccBtnState(createAccButtonEnabled, createAccButtonText);
}

// Store device unique identifier in registry
void Signup::storeDeviceId() {
    if (!settings->contains(DEVICE_ID_SETTINGS_KEY))
        settings->setValue(DEVICE_ID_SETTINGS_KEY, deviceInfo.getDeviceId());
}

// Get device unique identifier in registry
QString Signup::getDeviceIdLocally() const {
    return settings->value(DEVICE_ID_SETTINGS_KEY).toString();
}

// Slots
void Signup::onUsernameValidated(bool isValid) {
    validationStatus["username"] = isValid;
    checkValidationStatus();
}

void Signup::onEmailValidated(bool isValid) {
    validationStatus["email"] = isValid;
    checkValidationStatus();
}

void Signup::onPwdValidated(bool isValid) {
    validationStatus["password"] = isValid;
    checkValidationStatus();
}

void Signup::onNameValidated(bool isValid) {
    validationStatus["fullName"] = isValid;
    checkValidationStatus();
}

void Signup::onValidationDone(bool isValidationDone) {
    accountSignupWidget->createAccountButton()->setEnabled(isValidationDone);
}

void Signup::onTCBoxCheck(bool checked) {
    validationStatus["acceptedTC"] = checked;
    checkValidationStatus();
}

void Signup::onCreateAccBtnClicked() {
    Utils::InternetConnectivity::instance().runIfOnline([this](){    
        updateCreateAccBtnState(false, "");
        storeCredentials();
    }, this, errorDialogManager);
}

void Signup::onErrorDialogActionBtnClicked(const QString &key) {
    // future use
}

// Helper - Updating create account button state
void Signup::updateCreateAccBtnState(bool isEnabled, const QString &text) {
    accountSignupWidget->createAccountButton()->setEnabled(isEnabled);
    accountSignupWidget->createAccountButton()->setText(text);
}