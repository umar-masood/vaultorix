#include "TokenManager.h"
#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../utils/Utils.h"

using Core::Services::Auth::TokenManager;

TokenManager::TokenManager(QObject *parent) : QObject(parent) {
    // Network Manager
    manager = new QNetworkAccessManager(this);
}

TokenManager* TokenManager::instance() {
    static auto* token_manager = new TokenManager(qApp);
    return token_manager;
}

void TokenManager::extractTokens(QJsonObject &responseObj) {
    // Storing tokens
    _accessToken = responseObj["access_token"].toVariant().toByteArray();
    _refreshToken = responseObj["refresh_token"].toVariant().toByteArray();

    // Storing issued time 
    _accessTokenIssuedAt = responseObj["issued_at"].toVariant().toLongLong();
    _refreshTokenIssuedAt = _accessTokenIssuedAt; // Initially, both are same after signed in

    // Clearing
    responseObj = QJsonObject();
}

QNetworkRequest TokenManager::configureRequest(const QString &route) {
    QUrl url(route);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("accept", "application/json");
    request.setTransferTimeout(REQUEST_TIMEOUT);
    return request;
}

std::optional<QJsonDocument> TokenManager::parseNetworkReply(QNetworkReply *reply) {
    QJsonDocument doc;

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Network request failed:" << reply->errorString();
    } else {
        QJsonParseError parseError;
        doc = QJsonDocument::fromJson(reply->readAll(), &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            qDebug() << "JSON parsing error:" << parseError.errorString();
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
        qDebug() << "Failed to refesh the Access token, it's not available in memory.";
        return;
    }

    // Sending request to server
    QNetworkReply *reply = manager->post(configureRequest(route(APIRoutes::REFRESH)), refreshTokenJson().toJson());
    
    // Server Reply
    connect(reply, &QNetworkReply::finished, this, [this, reply]{        
        auto parseResponse = parseNetworkReply(reply);
        if (!parseResponse)
            return;

        QJsonObject mainObj = parseResponse.value().object();
        if (mainObj.contains("access_token") && mainObj.contains("issued_at")) {
            // Storing new access token in memory
            _accessToken = mainObj["access_token"].toVariant().toByteArray();

            // Storing issued time
            _accessTokenIssuedAt = mainObj["issued_at"].toVariant().toLongLong();

            // Emitting signal
            emit accessTokenRefreshed();

        } else if (mainObj.contains("status_code") && mainObj.contains("message")) {
            qDebug() << mainObj["status_code"].toString() << "  " << mainObj["message"].toString();
        } 
    });
}

void TokenManager::revokeRefreshToken() {
    if (_refreshToken.isEmpty()) {
        qDebug() << "Refresh token does not exists in memory for revoking.";
        return;
    }

    // Sending request to server
    QNetworkReply *reply = manager->post(configureRequest(route(APIRoutes::LOGOUT)), refreshTokenJson().toJson());
    
    // Server Reply
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        auto parseResponse = parseNetworkReply(reply);
        if (!parseResponse)
            return;

        QJsonObject mainObj = parseResponse.value().object();
        int statusCode = mainObj["status_code"].toInt();
        QString message = mainObj["message"].toString();

        if (statusCode == 200) {
            // Clearing refresh and access token from memory and resetting time
            Utils::cleanupMemory(_refreshToken);
            Utils::cleanupMemory(_accessToken);

            _accessTokenIssuedAt = 0;
            _refreshTokenIssuedAt = 0;
        }

        qDebug() << statusCode << "  " << message;
    });
}

bool TokenManager::isTokenExpired(const TokenType &type) {
    qint64 now = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();

    if (type == TokenType::Access) {
        if (now >= _accessTokenIssuedAt + ACCESS_TOKEN_EXPIRY) {
            qDebug() << "Access token has been expired.";
            return true;
        }
    } else {
        if (now >= _refreshTokenIssuedAt + REFRESH_TOKEN_EXPIRY) {
            qDebug() << "Refresh token has been expired.";
            return true;
        }
    }

    return false;
}

void TokenManager::sendRequest(const QString &route, const QByteArray &data, 
                               QNetworkAccessManager::Operation mode, 
                               std::function<void(const QJsonObject &)> callable) 
{
    auto makeRequest = [this, route, mode, data, callable](){
        // Configuring Network Request
        QNetworkRequest request = configureRequest(route);

        // Applying Access Token
        request.setRawHeader("Authorization", "Bearer " + accessToken());

        // Attaching data to request
        QNetworkReply *reply = nullptr;
        switch (mode) {
            case QNetworkAccessManager::PostOperation:
                reply = manager->post(request, data);
                break;
            case QNetworkAccessManager::GetOperation:
                reply = manager->get(request);
                break;
            case QNetworkAccessManager::PutOperation:
                reply = manager->put(request, data);
                break;
            case QNetworkAccessManager::DeleteOperation:
                reply = manager->deleteResource(request);
                break;
            default:
                qDebug() << "Unsupported HTTP Method.";
                return;
        }

        // Server Reply
        connect(reply, &QNetworkReply::finished, this, [this, reply, callable](){
            auto doc = parseNetworkReply(reply);
            if (!doc)
                return;
            
            QJsonObject mainObj = doc.value().object();
            callable(mainObj); // Calling user provided function
        });
    };

    // Handling expired refresh token
    if (isTokenExpired(TokenType::Refresh)) {
        emit sessionExpired();
        connect(this, &TokenManager::sessionExpired, this, &QApplication::quit);
        return;
    }

    // Handling expired access token
    if (isTokenExpired(TokenType::Access)) {
        
        // Storing copy of lambda object with its current state
        pendingRequests.push_back(makeRequest); 

        if (!isRefreshingAccessToken) {
            isRefreshingAccessToken = true;

            refreshAccessToken();

            connect(this, &TokenManager::accessTokenRefreshed, this, [this, makeRequest](){
                isRefreshingAccessToken = false;

                // Executing queued requests
                for (auto &req : pendingRequests)
                    req();

                // Clearing requests container
                pendingRequests.clear();

            }, Qt::SingleShotConnection);
        }

    } else
        makeRequest();
}

QByteArray TokenManager::refreshToken() const {
    return _refreshToken;
}

QByteArray TokenManager::accessToken() const {
    return _accessToken;
}