#include "TokenManager.h"
#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../utils/Utils.h"

using Core::TokenManager;

TokenManager::TokenManager(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);

    connect(this, &TokenManager::accessTokenRefreshed, this, [this]() {
        refreshLock.storeRelaxed(0);
        
        auto queue = std::move(pendingRequests);
        pendingRequests.clear();

        for (auto &req : queue)
            req();
        
        INFO_HERE("Access token has been regenerated.");
    });

    connect(this, &TokenManager::sessionExpired, qApp, &QApplication::quit);
}

TokenManager* TokenManager::instance() {
    static auto* token_manager = new TokenManager(qApp);
    return token_manager;
}

void TokenManager::extractTokens(const QJsonObject &responseObj) {
    QJsonObject tokensObj = responseObj["tokens"].toObject();

    // Storing tokens
    _accessToken = tokensObj["access_token"].toVariant().toByteArray();
    _refreshToken = tokensObj["refresh_token"].toVariant().toByteArray();

    // Storing issued time 
    _accessTokenIssuedAt = tokensObj["issued_at"].toVariant().toLongLong();
    _refreshTokenIssuedAt = _accessTokenIssuedAt;

    // Clearing
    tokensObj = QJsonObject();
}

QNetworkRequest TokenManager::configureRequest(const QString &route, bool jsonRequest) {
    QUrl url(route);
    QNetworkRequest request(url);

    if (jsonRequest)
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    request.setRawHeader("accept", "application/json");
    request.setTransferTimeout(REQUEST_TIMEOUT);
    return request;
}

std::optional<QJsonDocument> TokenManager::parseNetworkReply(QNetworkReply *reply) {
    QJsonDocument doc;

    if (reply->error() != QNetworkReply::NoError) {
        ERROR_HERE("Network request failed: " + reply->errorString());
    } else {
        QJsonParseError parseError;
        doc = QJsonDocument::fromJson(reply->readAll(), &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            ERROR_HERE("JSON parsing error.");
        } else {
            reply->deleteLater();
            return doc;
        }
    }

    reply->deleteLater();
    return std::nullopt;
}

QJsonDocument TokenManager::refreshTokenJson() {
    QJsonObject mainObj;
    mainObj["refresh_token"] = QString::fromUtf8(_refreshToken);
    return QJsonDocument(mainObj);
}

void TokenManager::refreshAccessToken() {
    if (_refreshToken.isEmpty()){
        ERROR_HERE("Failed to refesh the Access token, it's not available in memory.");
        return;
    }

    // Sending request to server
    QNetworkReply *reply = manager->post(configureRequest(route(APIRoutes::REFRESH), true), refreshTokenJson().toJson());
    
    // Server Reply
    connect(reply, &QNetworkReply::finished, this, [this, reply]{        
        auto parseResponse = parseNetworkReply(reply);
        if (!parseResponse)
            return;

        QJsonObject mainObj = parseResponse.value().object();

        if (mainObj.contains("access_token") && mainObj.contains("issued_at") && mainObj.contains("access_token_expires_in")) {
            // Storing new access token in memory
            _accessToken = mainObj["access_token"].toVariant().toByteArray();

            // Storing issued time
            _accessTokenIssuedAt = mainObj["issued_at"].toVariant().toLongLong();

            // Emitting signal
            emit accessTokenRefreshed();

        } else if (mainObj.contains("status_code") && mainObj.contains("message")) {
            DEBUG_HERE(mainObj["status_code"].toString() << "  " << mainObj["message"].toString());
        } 
    });
}

void TokenManager::revokeRefreshToken() {
    if (_refreshToken.isEmpty()) {
        ERROR_HERE("Refresh token does not exists in memory for revoking.");
        return;
    }

    // Sending request to server
    QNetworkReply *reply = manager->post(configureRequest(route(APIRoutes::LOGOUT), true), refreshTokenJson().toJson());
    
    // Server Reply
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        auto parseResponse = parseNetworkReply(reply);
        if (!parseResponse)
            return;

        QJsonObject mainObj = parseResponse.value().object();
        int statusCode = mainObj["status_code"].toInt();
        QString message = mainObj["message"].toString();

        if (statusCode == 200) {

            Utils::cleanupMemory(_refreshToken);
            Utils::cleanupMemory(_accessToken);

            _accessTokenIssuedAt = 0;
            _refreshTokenIssuedAt = 0;
        }

        DEBUG_HERE(QString::number(statusCode) + "   " + message);
    });
}

bool TokenManager::isTokenExpired(const TokenType &type) {
    qint64 now = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();

    if (type == TokenType::Access) {
        if (now >= _accessTokenIssuedAt + ACCESS_TOKEN_EXPIRY) {
            WARN_HERE("Access token is expired");
            return true;
        }
    } else {
        if (now >= _refreshTokenIssuedAt + REFRESH_TOKEN_EXPIRY) {
            WARN_HERE("Refresh token is expired - exiting...");
            return true;
        }
    }

    return false;
}

void TokenManager::performRequest(const QString &route, 
                                  std::function<QNetworkReply *(const QNetworkRequest &request)> requestCallable,    QNetworkAccessManager::Operation mode,
                                  bool jsonRequest,
                                  std::function<void(const QJsonObject &)> responseCallable,
                                  std::function<void(QNetworkReply *reply)> networkRequestFailureCallable)
{
    auto makeRequest = [this, route, mode, responseCallable, requestCallable, networkRequestFailureCallable, jsonRequest](){
        // Configuring Network Request
        QNetworkRequest request = configureRequest(route, jsonRequest);
        request.setTransferTimeout(REQUEST_TIMEOUT);

        // Applying Access Token
        request.setRawHeader("Authorization", "Bearer " + accessToken());

        // Attaching data to request and deciding request operation callable
        QNetworkReply *reply = requestCallable(request);
        if (!reply)
            return;

        // Request Error Handling
        connect(reply, &QNetworkReply::errorOccurred, this, [reply, networkRequestFailureCallable](QNetworkReply::NetworkError) {
            if (networkRequestFailureCallable) {
                networkRequestFailureCallable(reply);
            } else {
                WARN_HERE("Network Request Error: " + reply->errorString());
            }
        });

        // Server Reply
        connect(reply, &QNetworkReply::finished, this, [this, reply, responseCallable](){
            auto doc = parseNetworkReply(reply);
            if (!doc)
                return;

            
            if (!responseCallable) {
                WARN_HERE("Response callback is empty.");
                return;
            }
            
            QJsonObject mainObj = doc.value().object();
            qDebug() << mainObj;
            responseCallable(mainObj); // Calling user provided function
        });
    };

    // Handling expired refresh token
    if (isTokenExpired(TokenType::Refresh)) {
        emit sessionExpired();
        return;
    }

    // Handling expired access token
    if (isTokenExpired(TokenType::Access)) {
        
        // Storing copy of lambda object with its current state
        pendingRequests.push_back(makeRequest); 

        if (refreshLock.testAndSetRelaxed(0, 1)) {
            INFO_HERE("Access token is being generated.");
            refreshAccessToken();
        }

    } else
        makeRequest();
}

void TokenManager::sendRequest(const QString &route, 
                               const QByteArray &data, 
                               QNetworkAccessManager::Operation mode, 
                               std::function<void(const QJsonObject &)> responseCallable,
                               std::function<void(QNetworkReply *reply)> networkRequestFailureCallable
                            ) 
{
    performRequest(
        route,
        [this, mode, data](const QNetworkRequest &request){
            switch(mode) {
                case QNetworkAccessManager::PostOperation:
                    return manager->post(request, data);

                case QNetworkAccessManager::GetOperation:
                    return manager->get(request);

                case QNetworkAccessManager::PutOperation:
                    return manager->put(request, data);

                case QNetworkAccessManager::DeleteOperation:
                    return manager->deleteResource(request);

                default:
                    ERROR_HERE("Unsupported HTTP method.");
                    return static_cast<QNetworkReply *>(nullptr);
            };
        },
        mode,
        true,
        responseCallable,
        networkRequestFailureCallable
    );
}

void TokenManager::sendMultipartRequest(const QString &route, 
                                        std::function<QHttpMultiPart*()> multipartCallable,
                                        QNetworkAccessManager::Operation mode,
                                        std::function<void(const QJsonObject &)> responseCallable,
                                        std::function<void(QNetworkReply *reply)> networkRequestFailureCallable
                                    )
{
    performRequest(
        route,
        [this, mode, multipartCallable](const QNetworkRequest &request){
            QNetworkReply *reply = nullptr;
            QHttpMultiPart *multipart = multipartCallable();
            if (!multipart)
                return static_cast<QNetworkReply *>(nullptr);

            switch(mode) {
                case QNetworkAccessManager::PostOperation:
                    reply = manager->post(request, multipart);
                    break;

                case QNetworkAccessManager::PutOperation:
                    reply = manager->put(request, multipart);
                    break;

                default:
                    ERROR_HERE("Unsupported HTTP method.");
            };

            if (!reply) {
                delete multipart;
                return static_cast<QNetworkReply *>(nullptr);
            }
            
            multipart->setParent(reply);
            return reply;
        },
        mode,
        false,
        responseCallable,
        networkRequestFailureCallable
    );
}

QByteArray TokenManager::refreshToken() const { return _refreshToken; }
QByteArray TokenManager::accessToken() const { return _accessToken; }