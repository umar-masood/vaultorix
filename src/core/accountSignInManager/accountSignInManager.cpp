#include "accountSignInManager.h"

AccountSignInManager::AccountSignInManager(AccountWindow *accountWindow, QObject *parent) : QObject(parent) {
    handleDialogs(accountWindow);

    /* Network Manager */
    manager = new QNetworkAccessManager(this);

    connect(this, &AccountSignInManager::verifiedCredentials, this, &AccountSignInManager::onVerifiedCredentials);
    connect(this, &AccountSignInManager::maxLimitReached, this, &AccountSignInManager::onMaxLimitReached);
    connect(this, &AccountSignInManager::somethingWentWrong, this, &AccountSignInManager::onSomethingWentWrong);
    connect(this, &AccountSignInManager::invalidCredentials, this, &AccountSignInManager::onInvalidCredentials);
}

void AccountSignInManager::setAccountSignInObject(AccountSignIn *accountSignInObject) {
    if (!accountSignInObject)
        return;

    this->as = accountSignInObject;
    connect(as, &AccountSignIn::signInClicked, this, &AccountSignInManager::onSignInClicked);
    connect(as, &AccountSignIn::cancelClicked, this, &AccountSignInManager::onCancelClicked);
}

void AccountSignInManager::handleDialogs(AccountWindow *accountWindow) {
    /* Error Dialogs */
    if (!accountWindow)
        return;

    // Invalid Credentials Error
    invalidCredentialsErrorWidget = new Error(dialogSize, wrongCredentialsText, wrongCredentialsIcon, wrongCredentialsIcon, illustrationSize);
    invalidCredentialsErrorDialog = new Dialog(invalidCredentialsErrorWidget, accountWindow->subWindow(), false);
    connect(invalidCredentialsErrorWidget->actionButton(), &Button::clicked, this, &AccountSignInManager::onWrongCredentialsDailogActionBtnClicked);

    // Max Attempts Error
    maxAttemptsErrorWidget = new Error(dialogSize, maxAttemptsText, maxAttemptsIcon, maxAttemptsIcon, illustrationSize);
    maxAttemptsErrorDialog = new Dialog(maxAttemptsErrorWidget, accountWindow->subWindow(), false);
    connect(maxAttemptsErrorWidget->actionButton(), &Button::clicked, this, &AccountSignInManager::onMaxAttemptsDailogActionBtnClicked);

    // Something Went Wrong Error
    somethingWentWrongErrorWidget = new Error(dialogSize, somethingWentWrongText, somethingWentWrongIcon, somethingWentWrongIcon, illustrationSize);
    somethingWentWrongErrorDialog = new Dialog(somethingWentWrongErrorWidget, accountWindow->subWindow(), false);
    connect(somethingWentWrongErrorWidget->actionButton(), &Button::clicked, this, &AccountSignInManager::onSomethingWentWrongDailogActionBtnClicked);

    // Giving All Dialogs to Account Window for theme changing
    accountWindow->setSubWidgets({invalidCredentialsErrorDialog, maxAttemptsErrorDialog, maxAttemptsErrorWidget, invalidCredentialsErrorWidget, somethingWentWrongErrorDialog, somethingWentWrongErrorWidget});
}

void AccountSignInManager::verifyCredentials() {
    QUrl url("http://127.0.0.1:8000/check-credentials");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("api_key", API_KEY.toUtf8());

    QJsonObject mainObj;
    mainObj["username"] = QString::fromUtf8(this->username);
    mainObj["password"] = QString::fromUtf8(this->password);

    ValidatorUtils::cleanupMemory(this->username);
    ValidatorUtils::cleanupMemory(this->password);

    QJsonDocument doc(mainObj);
    QNetworkReply *reply = manager->post(request, doc.toJson());

    connect(reply, &QNetworkReply::finished, [this, reply](){
        if (!reply)
            return;

        if (reply->error() != QNetworkReply::NoError) {
            // Error detected
            reply->deleteLater();
            qDebug() << "Error :  " << reply->errorString() << "\n";
            emit somethingWentWrong();
            return;
        }

        QByteArray data = reply->readAll();
        reply->deleteLater();

        if (data.isEmpty()) {
            qDebug() << "Data is empty." << "\n";
            return;
        }

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "Json parsing error." << "\n";
            return;
        }

        if (!jsonDoc.isObject()) {
            qDebug() << "Invalid json data" << "\n";
            return;
        }

        QJsonObject obj = jsonDoc.object();
        message = obj["message"].toString();
        statusCode = obj["status_code"].toInt();
        
        qDebug() << "Verification response: " << message << " (Status Code: " << statusCode << ")\n";

        if (statusCode == 403)
            emit maxLimitReached();
        else if (statusCode == 200)
            emit verifiedCredentials();
        else if (statusCode == 400)
            emit invalidCredentials();
        else
            emit somethingWentWrong();
    });
}

void AccountSignInManager::onCancelClicked() {
    QApplication::quit();
}

void AccountSignInManager::onVerifiedCredentials() {
    qDebug() << "Credentials are verified." << "\n";
    updateSignInBtnState(false, "Signed In");
}

void AccountSignInManager::onMaxLimitReached() {
    qDebug() << "Max Limit Reached." << "\n";
    if (maxAttemptsErrorDialog)
        maxAttemptsErrorDialog->show();
    updateSignInBtnState(false, "Sign In");

}

void AccountSignInManager::onSomethingWentWrong() {
    qDebug() << "Something went wrong." << "\n";
    updateSignInBtnState(true, "Sign In");
    if (somethingWentWrongErrorDialog)
        somethingWentWrongErrorDialog->show();
}

void AccountSignInManager::onInvalidCredentials() {
    qDebug() << "Invalid Credentials." << "\n";
    if (invalidCredentialsErrorDialog)
        invalidCredentialsErrorDialog->show();
    updateSignInBtnState(true, "Sign In");
}

void AccountSignInManager::onWrongCredentialsDailogActionBtnClicked() {
    if (invalidCredentialsErrorDialog && invalidCredentialsErrorDialog->isVisible())
        invalidCredentialsErrorDialog->close();
}

void AccountSignInManager::onMaxAttemptsDailogActionBtnClicked(){
    if (maxAttemptsErrorDialog && maxAttemptsErrorDialog->isVisible())
        QApplication::quit();
}

void AccountSignInManager::onSomethingWentWrongDailogActionBtnClicked() {
    if (somethingWentWrongErrorDialog && somethingWentWrongErrorDialog->isVisible())
        somethingWentWrongErrorDialog->close();
}

void AccountSignInManager::onSignInClicked() {
    if (!this->as) 
        return;

    this->username = as->usernameField()->text().toUtf8();
    this->password = as->pwdField()->text().toUtf8();

    if (this->username.isEmpty() || this->password.isEmpty())
        return;

    updateSignInBtnState(false, "Signing In...");
    verifyCredentials();
}

void AccountSignInManager::updateSignInBtnState(bool isEnabled, QString text) {
    as->signInButton()->setEnabled(isEnabled);
    as->signInButton()->setText(text);
}