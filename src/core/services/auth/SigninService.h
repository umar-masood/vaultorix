#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

namespace Core {
    class SigninService : public QObject {
        Q_OBJECT

        public:
        enum class SignInError {
            InvalidCredentials,
            MaxAttemptsLimitReached,
            AccessDenied,
            SomethingWentWrong
        };

        explicit SigninService(QObject *parent = nullptr);
        void verifyCredentials(const QString &username, const QString &password);

        private:
        // API Key
        const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";

        // Network Manager
        QNetworkAccessManager *manager = nullptr;

        // Signals
        signals:
        void verificationRequired(const QString &email, const QString &authType);
        void failedToSignIn(const SignInError &error);
        void signedIn(const QJsonObject &obj);
    };
};