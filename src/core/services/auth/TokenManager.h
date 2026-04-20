#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QString>
#include <QDateTime>
#include <QSettings>
#include <QApplication>
#include <optional>
#include <functional>
#include <vector>

namespace Core::Services::Auth {
    class TokenManager : public QObject {
        Q_OBJECT
        TokenManager(QObject *parent = nullptr);

        public:
        static TokenManager *instance();

        enum class TokenType {
            Refresh,
            Access
        };

        /**
         * Sends request to Server with the following details:
         * - Route (URL)
         * - Data to send
         * - Network Operation Mode: (PostOperation, GetOperation, PutOperation, DeleteOperation)
         * - Callable (Lambda or any other function)
         */
        void sendRequest(const QString &route, const QByteArray &data,
                         QNetworkAccessManager::Operation mode = QNetworkAccessManager::PostOperation,
                         std::function<void(const QJsonObject &)> callable = nullptr);

        /**
         * Get Access and Refresh Token from server login response
         */
        void extractTokens(QJsonObject &responseObj);

        /**
         * Refresh the access token when it will get expired.
         */
        void refreshAccessToken();

        /**
         * Revokes Refresh Roken after being logged out
         */
        void revokeRefreshToken();

        /**
         * Checks whether the token has been expired?
         * @returns True if expired otherwise False
         */
        bool isTokenExpired(const TokenType &type);

        QByteArray refreshToken() const;
        QByteArray accessToken() const;

        signals:
        void accessTokenRefreshed();
        void sessionExpired();

        private:
        QByteArray _accessToken, _refreshToken;
        qint64 _accessTokenIssuedAt, _refreshTokenIssuedAt;
        QNetworkAccessManager *manager = nullptr;

        QJsonDocument refreshTokenJson();
        QNetworkRequest configureRequest(const QString &route);
        std::optional<QJsonDocument> parseNetworkReply(QNetworkReply *reply);

        bool isRefreshingAccessToken = false;
        std::vector<std::function<void()>> pendingRequests;
    };
};