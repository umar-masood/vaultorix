#pragma once

#include "../../ui/accountSignIn/AccountSignIn.h"
#include "../../ui/accountWindow/AccountWindow.h"
#include "../validators/validatorUtils/validatorUtils.h"
#include "../../ui/dialogs/errorDialog/errorDialog.h"
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
#include <QMap>

using namespace DialogUtils;

class AccountSignInManager : public QObject {
    Q_OBJECT
    
public:
    explicit AccountSignInManager(AccountWindow *accountWindow = nullptr, QObject *parent = nullptr);
    void setAccountSignInObject(AccountSignIn *accountSignInObject);

private:
    const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";
    const QString API_URL = "https://www.umarcreations.site/check-credentials";
    
    AccountWindow *accountWindow = nullptr;
    AccountSignIn *as = nullptr;
    
    QByteArray username;
    QByteArray password;

    QNetworkAccessManager *manager = nullptr;
    QString message;
    int statusCode;

    ErrorDialogManager *errorDialogManager = nullptr;
    
    void verifyCredentials();
    void updateSignInBtnState(bool isEnabled, const QString &text);

signals:
    void verifiedCredentials();
    void maxLimitReached();
    void somethingWentWrong();
    void invalidCredentials();
    void accessDenied();
    void requestTimeout();

private slots:
    void onSignInClicked();
    void onCancelClicked() const;
    void onVerifiedCredentials();
    void onMaxLimitReached();
    void onSomethingWentWrong();
    void onInvalidCredentials();    
    void onAccessDenied();
    void onRequestTimeout();
    void onErrorDialogActionBtnClicked(const QString &key);
};
