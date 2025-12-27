#include "accountSignInManager.h"
#include <QApplication>
#include <QDebug>

AccountSignInManager::AccountSignInManager(AccountWindow *accountWindow, QObject *parent) : QObject(parent) {
    handleDialogs(accountWindow);

    // Network Manager
    manager = new QNetworkAccessManager(this);

    // Connect internal signals
    connect(this, &AccountSignInManager::verifiedCredentials, this, &AccountSignInManager::onVerifiedCredentials);
    connect(this, &AccountSignInManager::maxLimitReached, this, &AccountSignInManager::onMaxLimitReached);
    connect(this, &AccountSignInManager::somethingWentWrong, this, &AccountSignInManager::onSomethingWentWrong);
    connect(this, &AccountSignInManager::invalidCredentials, this, &AccountSignInManager::onInvalidCredentials);
    connect(this, &AccountSignInManager::accessDenied, this, &AccountSignInManager::onAccessDenied);
    connect(this, &AccountSignInManager::requestTimeout, this, &AccountSignInManager::onRequestTimeout);
}

void AccountSignInManager::setAccountSignInObject(AccountSignIn *accountSignInObject) {
    if (!accountSignInObject) return;

    this->as = accountSignInObject;
    connect(as, &AccountSignIn::signInClicked, this, &AccountSignInManager::onSignInClicked);
    connect(as, &AccountSignIn::cancelClicked, this, &AccountSignInManager::onCancelClicked);
}

void AccountSignInManager::handleDialogs(AccountWindow *accountWindow) {
    if (!accountWindow) return;
    this->accountWindow = accountWindow;

    createErrorDialog("InvalidCredentials", wrongCredentialsText, wrongCredentialsIcon, &AccountSignInManager::onErrorDialogActionBtnClicked);
    createErrorDialog("MaxAttempts", maxAttemptsText, maxAttemptsIcon, &AccountSignInManager::onErrorDialogActionBtnClicked);
    createErrorDialog("SomethingWentWrong", somethingWentWrongText, somethingWentWrongIcon, &AccountSignInManager::onErrorDialogActionBtnClicked);
    createErrorDialog("AccessDenied", accessDeniedText, accessDeniedIcon, &AccountSignInManager::onErrorDialogActionBtnClicked);
    createErrorDialog("RequestTimeout", timeoutText, timeoutIcon, &AccountSignInManager::onErrorDialogActionBtnClicked);

    // Provide all dialogs to AccountWindow for theme changes
    QList<QWidget *> dialogs;
    for (const auto &ed : errorDialogs) {
        dialogs << ed.dialog << ed.widget;
    }
    accountWindow->setSubWidgets(dialogs);
}

void AccountSignInManager::createErrorDialog(const QString &key, const QString &text, const QString &iconPath, void(AccountSignInManager::*actionBtnSlot)(const QString&)) {
    ErrorDialog ed;
    ed.widget = new Error(dialogSize, text, iconPath, iconPath, illustrationSize);
    ed.dialog = new Dialog(ed.widget, accountWindow->subWindow(), false);

    // Use lambda to pass the key to the slot
    connect(ed.widget->actionButton(), &Button::clicked, [this, key, actionBtnSlot]() {
        (this->*actionBtnSlot)(key);
    });

    errorDialogs[key] = ed;
}

void AccountSignInManager::onErrorDialogActionBtnClicked(const QString &key) {
    if (errorDialogs.contains(key) && errorDialogs[key].dialog) {
        if (errorDialogs[key].dialog->isVisible())
            errorDialogs[key].dialog->close();
    }

    if (key == "MaxAttempts") QApplication::quit();
}

void AccountSignInManager::showErrorDialog(const QString &key, bool isSignInBtnEnabled, const QString &text) {
    if (errorDialogs.contains(key) && errorDialogs[key].dialog) {
        if (!errorDialogs[key].dialog->isVisible())
            errorDialogs[key].dialog->show();
    }
    updateSignInBtnState(isSignInBtnEnabled, text);
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
            emit requestTimeout();
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            emit somethingWentWrong();
            return;
        }

        QByteArray data = reply->readAll();
        reply->deleteLater();

        if (data.isEmpty()) return;

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
            emit somethingWentWrong();
            return;
        }

        QJsonObject obj = jsonDoc.object();
        message = obj["message"].toString();
        statusCode = obj["status_code"].toInt();

        switch (statusCode) {
            case 200: emit verifiedCredentials(); break;
            case 400: emit invalidCredentials(); break;
            case 403: emit maxLimitReached(); break;
            case 511: emit accessDenied(); break;
            default: emit somethingWentWrong(); break;
        }
    });
}

void AccountSignInManager::onSignInClicked() {
    if (!as) return;

    username = as->usernameField()->text().toUtf8();
    password = as->pwdField()->text().toUtf8();

    if (username.isEmpty() || password.isEmpty()) return;

    updateSignInBtnState(false, "");
    verifyCredentials();
}

void AccountSignInManager::updateSignInBtnState(bool isEnabled, const QString &text) {
    as->signInButton()->setEnabled(isEnabled);
    as->signInButton()->setText(text);
}

void AccountSignInManager::onCancelClicked() const {
    QApplication::quit();
}

void AccountSignInManager::onVerifiedCredentials() {
    updateSignInBtnState(false, "Signed In");
    as->usernameField()->setText("");
    as->pwdField()->setText("");
}

void AccountSignInManager::onMaxLimitReached() { showErrorDialog("MaxAttempts", false); }
void AccountSignInManager::onSomethingWentWrong() { showErrorDialog("SomethingWentWrong"); }
void AccountSignInManager::onInvalidCredentials() { showErrorDialog("InvalidCredentials"); }
void AccountSignInManager::onAccessDenied() { showErrorDialog("AccessDenied"); }
void AccountSignInManager::onRequestTimeout() { showErrorDialog("RequestTimeout"); }
