#include "accountSignInManager.h"
#include <QApplication>
#include <QDebug>

AccountSignInManager::AccountSignInManager(AccountWindow *accountWindow, QObject *parent) : QObject(parent) {
    // Network Manager
    manager = new QNetworkAccessManager(this);

    // Error Dialogs Manager
    errorDialogManager = new ErrorDialogManager(accountWindow, this);
    connect(errorDialogManager, &ErrorDialogManager::actionTriggered, this, &AccountSignInManager::onErrorDialogActionBtnClicked);
}

void AccountSignInManager::setAccountSignInObject(AccountSignIn *accountSignInObject) {
    if (!accountSignInObject) return;

    this->as = accountSignInObject;
    
    connect(as, &AccountSignIn::signInClicked, this, &AccountSignInManager::onSignInClicked);
    connect(as, &AccountSignIn::cancelClicked, this, &AccountSignInManager::onCancelClicked);
}

void AccountSignInManager::onErrorDialogActionBtnClicked(const QString &key) {
    if (key == "MaxAttempts") 
        QApplication::quit();
}

void AccountSignInManager::verifyCredentials() {
    QUrl url(API_URL);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("api_key", API_KEY.toUtf8());
    request.setTransferTimeout(15000);

    QJsonObject mainObj;
    mainObj["username"] = QString::fromUtf8(this->username);
    mainObj["password"] = QString::fromUtf8(this->password);

    ValidatorUtils::cleanupMemory(this->username);
    ValidatorUtils::cleanupMemory(this->password);

    QJsonDocument doc(mainObj);
    QNetworkReply *reply = manager->post(request, doc.toJson());

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (!reply) return;

        if (reply->error() == QNetworkReply::TimeoutError) {
            reply->deleteLater();
            handleSignInError("RequestTimeout", true);
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            handleSignInError("SomethingWentWrong", true);
            return;
        }

        QByteArray data = reply->readAll();
        reply->deleteLater();

        if (data.isEmpty()) return;

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
            handleSignInError("SomethingWentWrong", true);
            return;
        }

        QJsonObject obj = jsonDoc.object();
        message = obj["message"].toString();
        statusCode = obj["status_code"].toInt();
        qDebug() << message << statusCode;

        QString name, email, username;
        if (obj.contains("fullname") && obj.contains("email") && obj.contains("username")) {
            name = obj["fullname"].toString();
            email = obj["email"].toString();
            username = obj["username"].toString();
        }

        switch (statusCode) {
            case 200: {
                updateSignInBtnState(false, "Signed In");
                as->usernameField()->setText("");
                as->usernameField()->setEnabled(false);
                as->passwordField()->setText("");
                as->passwordField()->setEnabled(false);
            }
            break;

            case 400:  handleSignInError("InvalidCredentials", true);         break;
            case 403:  handleSignInError("MaxAttempts");                      break;
            case 511:  handleSignInError("AccessDenied");                     break;
            case 513:  emit verificationNeeded(name, email, username);        break;
            default:   handleSignInError("SomethingWentWrong", true);
        }
    });
}

void AccountSignInManager::onSignInClicked() {
    if (!as) return;

    username = as->usernameField()->text().toUtf8();
    password = as->passwordField()->text().toUtf8();

    if (username.isEmpty() || password.isEmpty()) return;

    updateSignInBtnState(false, "");
    verifyCredentials();
}

void AccountSignInManager::updateSignInBtnState(bool isEnabled, const QString &text) {
    as->signInButton()->setEnabled(isEnabled);
    as->signInButton()->setText(text);
}

void AccountSignInManager::handleSignInError(const QString &errorName, bool isSignInButtonEnabled, const QString &signInButtonText) {
    errorDialogManager->show(errorName); 
    updateSignInBtnState(isSignInButtonEnabled, signInButtonText);
}

void AccountSignInManager::onCancelClicked() const { QApplication::quit(); }
