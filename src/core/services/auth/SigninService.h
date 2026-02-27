#pragma once

#include "../../../ui/auth/signin/Signin.h"
#include "../../../ui/auth/authWindow/AuthWindow.h"

#include "../../utils/ValidatorUtils.h"

#include "../../../ui/dialogs/errorDialog/ErrorDialog.h"
#include "../../../ui/components/Dialog.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QMap>

class SigninService : public QObject {
    Q_OBJECT
    
    public:
    explicit SigninService(AuthWindow *instance = nullptr, QObject *parent = nullptr);
    void setAccountSignin(Signin *instance);

    private:
    // API Crendentials
    const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";
    //const QString API_URL = "https://www.umarcreations.site/verify-credentials";
    const QString API_URL = "http://127.0.0.1:8000/verify-credentials";

    // Account Sign In Widget
    Signin *as = nullptr;
    
    // Stores Username & Password
    QByteArray username;
    QByteArray password;

    // Network Manager
    QNetworkAccessManager *manager = nullptr;

    // API Response
    QString message;
    int statusCode;

    // Errors Dialog Manager
    ErrorDialogManager *errorDialogManager = nullptr;
    
    // Helper Methods
    void verifyCredentials();
    void updateSignInBtnState(bool isEnabled, const QString &text);
    void handleSignInError(const QString &errorName, bool isSignInButtonEnabled = false, const QString &signInButtonText = "Sign In");

    // Slots
    void onSignInClicked();
    void onCancelClicked() const;
    void onErrorDialogActionBtnClicked(const QString &key); 
    
    // Signals
    signals:
    void verificationNeeded(const QString &name, const QString &email, const QString &username);
};
