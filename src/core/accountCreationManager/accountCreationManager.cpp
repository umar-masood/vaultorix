#include "AccountCreationManager.h"
#include <QDebug>

AccountCreationManager::AccountCreationManager(AccountWindow *accountWindow, QObject *parent) : QObject(parent), accountWindow(accountWindow) {
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

void AccountCreationManager::setAccountCreateObject(AccountCreate *accountCreateObj) {
    if (!accountCreateObj) return;
    accountCreate = accountCreateObj;
    
    emailValidator->setAccountCreateObject(accountCreate);
    usernameValidator->setAccountCreateObject(accountCreate);
    pwdValidator->setAccountCreateObject(accountCreate);
    nameValidator->setAccountCreateObject(accountCreate);

    setupConnections();
}

void AccountCreationManager::setupConnections() {
    connect(usernameValidator, &GetUsername::usernameValidated, this, &AccountCreationManager::onUsernameValidated);
    connect(emailValidator, &GetEmail::emailValidated, this, &AccountCreationManager::onEmailValidated);
    connect(pwdValidator, &GetPassword::pwdValidated, this, &AccountCreationManager::onPwdValidated);
    connect(nameValidator, &GetName::nameValidated, this, &AccountCreationManager::onNameValidated);
    connect(this, &AccountCreationManager::validationDone, this, &AccountCreationManager::onValidationDone);
    connect(accountCreate->termsConditionsWidget(), &CheckWithBtn::boxChecked, this, &AccountCreationManager::onTCBoxCheck);
    connect(accountCreate->createAccountButton(), &Button::clicked, this, &AccountCreationManager::onCreateAccBtnClicked);
    connect(this, &AccountCreationManager::credentialsStoredSuccessfully, this, &AccountCreationManager::onCredentialsStoredSuccessfully);
    connect(errorDialogManager, &ErrorDialogManager::actionTriggered, this, &AccountCreationManager::onErrorDialogActionBtnClicked);
}

void AccountCreationManager::checkValidationStatus() {
    for (auto it = validationStatus.begin(); it != validationStatus.end(); ++it) {
        if (!it->second) {
            emit validationDone(false);
            return;
        }
    }
    emit validationDone(true);
}

QJsonObject AccountCreationManager::getCredentials() {
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

    qDebug() << getDeviceIdLocally();
    
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

void AccountCreationManager::storeCredentials() {
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

        if (statusCode == 200) 
            emit credentialsStoredSuccessfully(); 
        else if (statusCode == 403) 
            handleCreateAccError("MaxAttempts");
        else if (statusCode == 513) 
            handleCreateAccError("FurtherAttemptBlocked");
        else 
            handleCreateAccError("SomethingWentWrong", true);
    });
}

void AccountCreationManager::handleCreateAccError(const QString &errorName, bool createAccButtonEnabled, const QString &createAccButtonText) {
    errorDialogManager->show(errorName); 
    errorDialogManager->show("FurtherAttemptBlocked");
    errorDialogManager->show("SomethingWentWrong");
    errorDialogManager->show("MaxAttempts");
    errorDialogManager->show("InvalidCredentials");
    errorDialogManager->show("AccessDenied");
    errorDialogManager->show("RequestTimeout");
    updateCreateAccBtnState(createAccButtonEnabled, createAccButtonText);
}

void AccountCreationManager::storeDeviceId() {
    if (!settings->contains("deviceId"))
        settings->setValue("deviceId", deviceInfo.getDeviceId());
}

QString AccountCreationManager::getDeviceIdLocally() const {
    return settings->value("deviceId").toString();
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
    checkValidationStatus();
}

void AccountCreationManager::onNameValidated(bool isValid) {
    validationStatus["fullName"] = isValid;
    checkValidationStatus();
}

void AccountCreationManager::onValidationDone(bool isValidationDone) {
    accountCreate->createAccountButton()->setEnabled(isValidationDone);
}

void AccountCreationManager::onTCBoxCheck(bool checked) {
    validationStatus["acceptedTC"] = checked;
    checkValidationStatus();
}

void AccountCreationManager::onCreateAccBtnClicked() {
    storeCredentials();
    updateCreateAccBtnState(false, "");
}

void AccountCreationManager::updateCreateAccBtnState(bool isEnabled, const QString &text) {
    accountCreate->createAccountButton()->setEnabled(isEnabled);
    accountCreate->createAccountButton()->setText(text);
}

void AccountCreationManager::onErrorDialogActionBtnClicked(const QString &key) {
    // future use
}

void AccountCreationManager::onCredentialsStoredSuccessfully() {
    qDebug() << "Credentials stored successfully!";
    QApplication::quit();
}
