#pragma once

#include "../../ui/accountSignIn/AccountSignIn.h"
#include "../../ui/accountWindow/AccountWindow.h"
#include "../validators/validatorUtils/validatorUtils.h"
#include "../../ui/dialogs/errorDialog/ErrorDialog.h"
#include "../../ui/components/Dialog.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QMap>

using namespace DialogUtils;

class AccountSignInManager : public QObject {
    Q_OBJECT
    
    public:
    explicit AccountSignInManager(AccountWindow *accountWindow = nullptr, QObject *parent = nullptr);
    void setAccountSignInObject(AccountSignIn *accountSignInObject);

    private:
    // API Crendentials
    const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";
    //const QString API_URL = "https://www.umarcreations.site/check-credentials";
    const QString API_URL = "http://127.0.0.1:8000/check-credentials";

    // Current Account Window
    AccountWindow *accountWindow = nullptr;

    // Account Sign In Widget
    AccountSignIn *as = nullptr;
    
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
