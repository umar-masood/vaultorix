#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QMap>

class ErrorDialogManager;
class Signin;
class AuthWindow;

class SigninService : public QObject {
    Q_OBJECT
    
    public:
    explicit SigninService(AuthWindow *instance = nullptr, QObject *parent = nullptr);
    void setAccountSignin(Signin *instance);

    private:
    // API Crendentials
    const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";

    // Account Sign In Widget
    Signin *as = nullptr;
    
    // Stores Username & Password
    QByteArray username, password;

    // Network Manager
    QNetworkAccessManager *manager = nullptr;

    // Auth Window
    AuthWindow *aw = nullptr;

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
    void verificationNeeded(const QString &email);
};
