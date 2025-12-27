#include "accountCreationManager.h"

AccountCreationManager::AccountCreationManager(QObject *parent) : QObject(parent) {
    emailValidator = new GetEmail(this);
    usernameValidator = new GetUsername(this);
    pwdValidator = new GetPassword(this);
    nameValidator = new GetName(this); 
    
    // Network Manager
    manager = new QNetworkAccessManager(this);
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

void AccountCreationManager::storeCredentials() {
    QUrl url(API_URL);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("api_key", API_KEY.toUtf8());
    request.setTransferTimeout(15000);

    QJsonObject mainObj;
    
    QJsonObject userObj;
    userObj["full_name"] = "Ali";
    userObj["username"] = "this->username";
    userObj["email_address"] = "this->email";
    userObj["password"] = "this->password";
    userObj["created_at"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    userObj["is_active"] = true;

    QJsonObject deviceObj;
    deviceObj["device_name"] = deviceInfo.getDeviceName();
    deviceObj["device_id"] = deviceInfo.getDeviceId();
    deviceObj["os_type"] = deviceInfo.getOSName();
    deviceObj["os_version"] = deviceInfo.getProductVersion();
    deviceObj["kernel_version"] = deviceInfo.getKernelVersion();

    QJsonObject appObj;
    appObj["version"] = QCoreApplication::applicationVersion();

    mainObj["user_info"] = userObj;
    mainObj["device_info"] = deviceObj;
    mainObj["app_info"] = appObj;

    //ValidatorUtils::cleanupMemory(this->username);
    //ValidatorUtils::cleanupMemory(this->password);

    QJsonDocument doc(mainObj);
    QNetworkReply *reply = manager->post(request, doc.toJson());

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (!reply) return;

        if (reply->error() == QNetworkReply::TimeoutError) {
            reply->deleteLater();
            //emit requestTimeout();
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            //emit somethingWentWrong();
            return;
        }

        QByteArray data = reply->readAll();
        reply->deleteLater();

        if (data.isEmpty()) return;

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
           // emit somethingWentWrong();
            return;
        }

        QJsonObject obj = jsonDoc.object();
        message = obj["message"].toString();
        statusCode = obj["status_code"].toInt();

        // switch (statusCode) {
        //     case 200: emit verifiedCredentials(); break;
        //     case 400: emit invalidCredentials(); break;
        //     case 403: emit maxLimitReached(); break;
        //     case 511: emit accessDenied(); break;
        //     default: emit somethingWentWrong(); break;
        // }
    });
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
