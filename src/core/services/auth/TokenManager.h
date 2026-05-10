#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QUrl>
#include <QString>
#include <QDateTime>
#include <QSettings>
#include <QApplication>
#include <optional>
#include <functional>
#include <QAtomicInt>
#include <vector>

namespace Core {
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
         * - Request Callable (Lambda or any other function)
         * - Network Request Failure Callable
         */
        void sendRequest(const QString &route, 
                         const QByteArray &data,
                         QNetworkAccessManager::Operation mode = QNetworkAccessManager::PostOperation,
                         std::function<void(const QJsonObject &)> responseCallable = nullptr,
                         std::function<void(QNetworkReply *reply)> networkRequestFailureCallable = nullptr
                        );

        void sendMultipartRequest(const QString &route, 
                                  std::function<QHttpMultiPart*()> multipartCallable,
                                  QNetworkAccessManager::Operation mode = QNetworkAccessManager::PostOperation,
                                  std::function<void(const QJsonObject &)> responseCallable = nullptr,
                                  std::function<void(QNetworkReply *reply)> networkRequestFailureCallable = nullptr
                                );
        /**
         * Get Access and Refresh Token from server login response
         */
        void extractTokens(const QJsonObject &responseObj);

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
        bool isJsonRequest = true;
        QAtomicInt refreshLock = 0;

        QJsonDocument refreshTokenJson();
        QNetworkRequest configureRequest(const QString &route, bool jsonRequest);
        std::optional<QJsonDocument> parseNetworkReply(QNetworkReply *reply);
        void performRequest(const QString &route,
                            std::function<QNetworkReply *(const QNetworkRequest &request)> requestCallable = nullptr,
                            QNetworkAccessManager::Operation mode = QNetworkAccessManager::PostOperation,
                            bool jsonRequest = true,
                            std::function<void(const QJsonObject &)> responseCallable = nullptr,
                            std::function<void(QNetworkReply *reply)> networkRequestFailureCallable = nullptr
                        );

        bool isRefreshingAccessToken = false;
        std::vector<std::function<void()>> pendingRequests;
    };
};