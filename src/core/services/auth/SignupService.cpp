#include "SignupService.h"
#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../utils/Utils.h"
#include <QDebug>

/* ========================================================================================= 
                              ACCOUNT CREATE MANAGER            
   ========================================================================================= */
SignupService::SignupService(AuthWindow *instance, QObject *parent) : QObject(parent), authWindow(authWindow) {
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

void SignupService::setAccountSignup(Signup *instance) {
    // If account create pointer does not initialized
    if (!instance) 
        return;
    
    // Initializing account create poimter
    accountSignup = instance;
    
    // Giving account create pointer to these member functions
    emailValidator->setAccountSignup(accountSignup);
    usernameValidator->setAccountSignup(accountSignup);
    passwordValidator->setAccountSignup(accountSignup);
    nameValidator->setAccountSignup(accountSignup);

    // Signal Slot Connections
    setupConnections();
}

void SignupService::setupConnections() {
    connect(usernameValidator, &GetUsername::usernameValidated, this, &SignupService::onUsernameValidated);
    connect(emailValidator, &GetEmail::emailValidated, this, &SignupService::onEmailValidated);
    connect(passwordValidator, &GetPassword::pwdValidated, this, &SignupService::onPwdValidated);
    connect(nameValidator, &GetName::nameValidated, this, &SignupService::onNameValidated);
    connect(this, &SignupService::validationDone, this, &SignupService::onValidationDone);
    connect(accountSignup->termsConditionsWidget(), &CheckWithBtn::boxChecked, this, &SignupService::onTCBoxCheck);
    connect(accountSignup->createAccountButton(), &Button::clicked, this, &SignupService::onCreateAccBtnClicked);
    connect(errorDialogManager, &ErrorDialogManager::actionTriggered, this, &SignupService::onErrorDialogActionBtnClicked);
}

// Check validation status for enable/disable create account button
void SignupService::checkValidationStatus() {
    for (auto it = validationStatus.begin(); it != validationStatus.end(); ++it) {
        if (!it->second) {
            emit validationDone(false);
            return;
        }
    }
    emit validationDone(true);
}

// Preparing json data for request
QJsonObject SignupService::getCredentials() {
    QJsonObject userObj;
    userObj["full_name"] = accountSignup->nameField()->text();
    userObj["username"] = accountSignup->usernameField()->text();
    userObj["email_address"] = accountSignup->emailField()->text();
    userObj["password"] = accountSignup->passwordField()->text();
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
void SignupService::storeCredentials() {
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
void SignupService::handleCreateAccError(const QString &errorName, bool createAccButtonEnabled, const QString &createAccButtonText) {
    errorDialogManager->show(errorName, "Auth"); 
    updateCreateAccBtnState(createAccButtonEnabled, createAccButtonText);
}

// Store device unique identifier in registry
void SignupService::storeDeviceId() {
    if (!settings->contains(DEVICE_ID_SETTINGS_KEY))
        settings->setValue(DEVICE_ID_SETTINGS_KEY, deviceInfo.getDeviceId());
}

// Get device unique identifier in registry
QString SignupService::getDeviceIdLocally() const {
    return settings->value(DEVICE_ID_SETTINGS_KEY).toString();
}

// Slots
void SignupService::onUsernameValidated(bool isValid) {
    validationStatus["username"] = isValid;
    checkValidationStatus();
}

void SignupService::onEmailValidated(bool isValid) {
    validationStatus["email"] = isValid;
    checkValidationStatus();
}

void SignupService::onPwdValidated(bool isValid) {
    validationStatus["password"] = isValid;
    checkValidationStatus();
}

void SignupService::onNameValidated(bool isValid) {
    validationStatus["fullName"] = isValid;
    checkValidationStatus();
}

void SignupService::onValidationDone(bool isValidationDone) {
    accountSignup->createAccountButton()->setEnabled(isValidationDone);
}

void SignupService::onTCBoxCheck(bool checked) {
    validationStatus["acceptedTC"] = checked;
    checkValidationStatus();
}

void SignupService::onCreateAccBtnClicked() {
    Utils::InternetConnectivity::instance().runIfOnline([this](){    
        updateCreateAccBtnState(false, "");
        storeCredentials();
    }, this, errorDialogManager);
}

void SignupService::onErrorDialogActionBtnClicked(const QString &key) {
    // future use
}

// Helper - Updating create account button state
void SignupService::updateCreateAccBtnState(bool isEnabled, const QString &text) {
    accountSignup->createAccountButton()->setEnabled(isEnabled);
    accountSignup->createAccountButton()->setText(text);
}