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

namespace Ui::Auth {
    class Signin;
    class AuthWindow;
};

namespace Core::Services::Auth {
    class Signin : public QObject {
        Q_OBJECT

        public:
        /*
            Constuctor Param - AuthWindow from Ui section
        */
        explicit Signin(Ui::Auth::AuthWindow *authWindow = nullptr, QObject *parent = nullptr);

        /**
         * This method will take the instance of current Account Sign in widget from Ui section
         */
        void setAccountSigninWidget(Ui::Auth::Signin *instance);

        private:
        // API Crendentials
        const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";

        // Account Sign in Widget
        Ui::Auth::Signin *signinWidget = nullptr;

        // Stores Username & Password
        QByteArray username, password;

        // Network Manager
        QNetworkAccessManager *manager = nullptr;

        // Auth Window Widget
        Ui::Auth::AuthWindow *authWindow = nullptr;

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
};