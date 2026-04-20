#include "Signin.h"
#include "TokenManager.h"

#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../../ui/auth/signin/Signin.h"
#include "../../../ui/auth/auth_window/AuthWindow.h"
#include "../../utils/Utils.h"
#include "../../../ui/dialogs/error_dialog/ErrorDialog.h"
#include "../../../ui/components/Dialog.h"
#include "../../../ui/vault/vault_window/VaultWindow.h"

#include <QApplication>
#include <QDebug>

using Core::Services::Auth::Signin;

Signin::Signin(Ui::Auth::AuthWindow *authWindow, QObject *parent) : QObject(parent) {
    // Network Manager
    manager = new QNetworkAccessManager(this);

    // Auth Window Widget
    this->authWindow = authWindow;

    // Error Dialogs Manager
    errorDialogManager = ErrorDialogManager::instance();
    connect(errorDialogManager, &ErrorDialogManager::actionTriggered, this, &Signin::onErrorDialogActionBtnClicked);
}

void Signin::setAccountSigninWidget(Ui::Auth::Signin *instance) {
    if (!instance) 
        return;

    signinWidget = instance;
    
    connect(signinWidget, &Ui::Auth::Signin::signInClicked, this, &Signin::onSignInClicked);
    connect(signinWidget, &Ui::Auth::Signin::cancelClicked, this, &Signin::onCancelClicked);
}

void Signin::onErrorDialogActionBtnClicked(const QString &key) {
    if (key == "MaxAttempts") 
        QApplication::quit();
}

void Signin::verifyCredentials() {
    QNetworkRequest request(QUrl(route(APIRoutes::SIGNIN)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("api_key", API_KEY.toUtf8());
    request.setTransferTimeout(REQUEST_TIMEOUT);

    QJsonObject mainObj;
    mainObj["username"] = QString::fromUtf8(this->username);
    mainObj["password"] = QString::fromUtf8(this->password);

    Utils::cleanupMemory(this->username);
    Utils::cleanupMemory(this->password);

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

        // Server Response
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

        // Handle Identity Verification if server asks
        QString email;
        if (obj.contains("email")) {
            email = obj["email"].toString();
            qDebug() << "Email Address Received signinWidget Verification Required Model from Server : " << email;
        }

        switch (statusCode) {
            case 200: {
                updateSignInBtnState(false, "Signed In");

                TokenManager::instance()->extractTokens(obj);

                if (authWindow) {
                    authWindow->close();
                    authWindow->deleteLater();
                }

                signinWidget->usernameField()->setText("");
                signinWidget->usernameField()->setEnabled(false);

                signinWidget->passwordField()->setText("");
                signinWidget->passwordField()->setEnabled(false);

                Ui::Vault::VaultWindow::instance()->show();
            }
            break;

            case 400:  handleSignInError("InvalidCredentials", true);         break;
            case 403:  handleSignInError("MaxAttempts");                      break;
            case 511:  handleSignInError("AccessDenied");                     break;
            case 513:  emit verificationNeeded(email);                        break;
            default:   handleSignInError("SomethingWentWrong", true);
        }
    });
}

void Signin::onSignInClicked() {
    if (!signinWidget) return;

    username = signinWidget->usernameField()->text().toUtf8();
    password = signinWidget->passwordField()->text().toUtf8();

    if (username.isEmpty() || password.isEmpty()) return;
    if (username.length() < 3 || password.length() < 8) return;

    Utils::InternetConnectivity::instance().runIfOnline([this](){    
        updateSignInBtnState(false, "");
        verifyCredentials();
    }, this, errorDialogManager);
}

void Signin::updateSignInBtnState(bool isEnabled, const QString &text) {
    signinWidget->signInButton()->setEnabled(isEnabled);
    signinWidget->signInButton()->setText(text);
}

void Signin::handleSignInError(const QString &errorName, bool isSignInButtonEnabled, const QString &signInButtonText) {
    errorDialogManager->show(errorName, "Auth"); 
    updateSignInBtnState(isSignInButtonEnabled, signInButtonText);
}

void Signin::onCancelClicked() const { QApplication::quit(); }
