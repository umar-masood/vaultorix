#include "AccountCreateManager.h"
#include <QDebug>

/* ========================================================================================= 
                              ACCOUNT CREATE MANAGER            
   ========================================================================================= */
AccountCreateManager::AccountCreateManager(AccountWindow *accountWindow, QObject *parent) : QObject(parent), accountWindow(accountWindow) {
    // Validators
    emailValidator = new GetEmail(this);
    usernameValidator = new GetUsername(this);
    pwdValidator = new GetPassword(this);
    nameValidator = new GetName(this);

    // Network Manager
    manager = new QNetworkAccessManager(this);

    // Settings
    settings = new QSettings(this);

    // Generate device id and stores it if it does not exists
    storeDeviceId();

    // Error Dialogs Manager
    errorDialogManager = new ErrorDialogManager(accountWindow, this);
}

void AccountCreateManager::setAccountCreateObject(AccountCreate *accountCreateObj) {
    // If account create pointer does not initialized
    if (!accountCreateObj) 
        return;
    
    // Initializing account create poimter
    accountCreate = accountCreateObj;
    
    // Giving account create pointer to these member functions
    emailValidator->setAccountCreateObject(accountCreate);
    usernameValidator->setAccountCreateObject(accountCreate);
    pwdValidator->setAccountCreateObject(accountCreate);
    nameValidator->setAccountCreateObject(accountCreate);

    // Signal Slot Connections
    setupConnections();
}

void AccountCreateManager::setupConnections() {
    connect(usernameValidator, &GetUsername::usernameValidated, this, &AccountCreateManager::onUsernameValidated);
    connect(emailValidator, &GetEmail::emailValidated, this, &AccountCreateManager::onEmailValidated);
    connect(pwdValidator, &GetPassword::pwdValidated, this, &AccountCreateManager::onPwdValidated);
    connect(nameValidator, &GetName::nameValidated, this, &AccountCreateManager::onNameValidated);
    connect(this, &AccountCreateManager::validationDone, this, &AccountCreateManager::onValidationDone);
    connect(accountCreate->termsConditionsWidget(), &CheckWithBtn::boxChecked, this, &AccountCreateManager::onTCBoxCheck);
    connect(accountCreate->createAccountButton(), &Button::clicked, this, &AccountCreateManager::onCreateAccBtnClicked);
    connect(errorDialogManager, &ErrorDialogManager::actionTriggered, this, &AccountCreateManager::onErrorDialogActionBtnClicked);
}

// Check validation status for enable/disable create account button
void AccountCreateManager::checkValidationStatus() {
    for (auto it = validationStatus.begin(); it != validationStatus.end(); ++it) {
        if (!it->second) {
            emit validationDone(false);
            return;
        }
    }
    emit validationDone(true);
}

// Preparing json data for request
QJsonObject AccountCreateManager::getCredentials() {
    QJsonObject userObj;
    userObj["full_name"] = accountCreate->nameField()->text();
    userObj["username"] = accountCreate->usernameField()->text();
    userObj["email_address"] = accountCreate->emailField()->text();
    userObj["password"] = accountCreate->passwordField()->text();
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
void AccountCreateManager::storeCredentials() {
    // QUrl url(API_URL);
    QUrl url("http://127.0.0.1:8000/store-credentials");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("api_key", API_KEY.toUtf8());
    request.setTransferTimeout(15000);

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
        else*/  // We will handle this later if we thinks it's necessary 
            handleCreateAccError("SomethingWentWrong", true);
    });
}

// Handling error dialogs and create account button status update
void AccountCreateManager::handleCreateAccError(const QString &errorName, bool createAccButtonEnabled, const QString &createAccButtonText) {
    errorDialogManager->show(errorName); 
    updateCreateAccBtnState(createAccButtonEnabled, createAccButtonText);
}

// Store device unique identifier in registry
void AccountCreateManager::storeDeviceId() {
    if (!settings->contains("deviceId"))
        settings->setValue("deviceId", deviceInfo.getDeviceId());
}

// Get device unique identifier in registry
QString AccountCreateManager::getDeviceIdLocally() const {
    return settings->value("deviceId").toString();
}

// Slots
void AccountCreateManager::onUsernameValidated(bool isValid) {
    validationStatus["username"] = isValid;
    checkValidationStatus();
}

void AccountCreateManager::onEmailValidated(bool isValid) {
    validationStatus["email"] = isValid;
    checkValidationStatus();
}

void AccountCreateManager::onPwdValidated(bool isValid) {
    validationStatus["password"] = isValid;
    checkValidationStatus();
}

void AccountCreateManager::onNameValidated(bool isValid) {
    validationStatus["fullName"] = isValid;
    checkValidationStatus();
}

void AccountCreateManager::onValidationDone(bool isValidationDone) {
    accountCreate->createAccountButton()->setEnabled(isValidationDone);
}

void AccountCreateManager::onTCBoxCheck(bool checked) {
    validationStatus["acceptedTC"] = checked;
    checkValidationStatus();
}

void AccountCreateManager::onCreateAccBtnClicked() {
    storeCredentials();
    updateCreateAccBtnState(false, "");
}

void AccountCreateManager::onErrorDialogActionBtnClicked(const QString &key) {
    // future use
}

// Helper - Updating create account button state
void AccountCreateManager::updateCreateAccBtnState(bool isEnabled, const QString &text) {
    accountCreate->createAccountButton()->setEnabled(isEnabled);
    accountCreate->createAccountButton()->setText(text);
}

