#pragma once

#include "../../ui/accountSignIn/AccountSignIn.h"
#include "../../ui/accountWindow/AccountWindow.h"
#include "../validators/validatorUtils/validatorUtils.h"
#include "../../ui/dialogs/error/Error.h"
#include "../../ui/components/Dialog.h"
#include "../../ui/dialogs/dialogsUtils.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>

using namespace DialogUtils;

class AccountSignInManager : public QObject {
    Q_OBJECT
    
    public:
    explicit AccountSignInManager(AccountWindow *accountWindow = nullptr, QObject *parent = nullptr);
    
    void setAccountSignInObject(AccountSignIn *accountSignInObject);

    private:
    const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";

    AccountSignIn *as = nullptr;
    QNetworkAccessManager *manager = nullptr;

    QByteArray username;
    QByteArray password;

    QString message;
    int statusCode;

    // Error Dialogs
    Error *invalidCredentialsErrorWidget = nullptr;
    Dialog *invalidCredentialsErrorDialog = nullptr;

    Error *maxAttemptsErrorWidget = nullptr;
    Dialog *maxAttemptsErrorDialog = nullptr;

    Error *somethingWentWrongErrorWidget = nullptr;
    Dialog *somethingWentWrongErrorDialog = nullptr;

    void handleDialogs(AccountWindow *accountWindow);
    void verifyCredentials();
    void updateSignInBtnState(bool isEnabled, QString text);

    signals:
    void verifiedCredentials();
    void maxLimitReached();
    void somethingWentWrong();
    void invalidCredentials();

    private slots:
    void onSignInClicked();
    void onCancelClicked();
    void onVerifiedCredentials();
    void onMaxLimitReached();
    void onSomethingWentWrong();
    void onInvalidCredentials();
    void onWrongCredentialsDailogActionBtnClicked();
    void onMaxAttemptsDailogActionBtnClicked();
    void onSomethingWentWrongDailogActionBtnClicked();
};