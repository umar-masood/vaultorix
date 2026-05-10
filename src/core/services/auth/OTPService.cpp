#include "OTPService.h"

#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../utils/Utils.h"

using Core::OTPService;

/* --------------  OTPService Implementation ---------------------  */
OTPService::OTPService(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

void OTPService::sendOTP(const QString &email) {
    // Request function
    auto requestFunction = [this, email]() {
        QJsonObject obj { {"email", email} };
        QJsonDocument doc(obj);

        // POST request
        QNetworkReply *reply = manager->post(
            configureNetworkRequest(route(APIRoutes::OTP_SEND)), 
            doc.toJson()
        );

        connect(reply, &QNetworkReply::finished, this, [this, reply](){
            // Network request error
            if (reply->error() != QNetworkReply::NoError) {
                ERROR_HERE("Failed to send OTP:  " + reply->errorString());
                emit somethingWentWrong(OTPAction::Send);
                reply->deleteLater();
                return;
            }

            // Response received from the server
            QByteArray response = reply->readAll();
            QJsonDocument r = QJsonDocument::fromJson(response);

            if (!r.isObject()) {
                reply->deleteLater();
                return;
            }

            // Main Object
            QJsonObject mainObj = r.object();

            // Server Errors
            int statusCode = mainObj["status_code"].toInt();
            if (statusCode > 0) {
                switch (statusCode) {
                    case 200:  emit OTPSent();                                break;
                    case 429:  emit maxLimitReached();                        break;
                    case 500:
                    case 512:
                    case 400:  emit somethingWentWrong(OTPAction::Send);      break;
                }
            }

            DEBUG_HERE(QString::number(statusCode) + "   " + mainObj["message"].toString());
            reply->deleteLater();
        });
    };

    Utils::InternetConnectivity::instance().runIfOnline(
        [this, requestFunction]() {
            requestFunction();
        },
        this,
        "Auth",
        [this](){
            emit noInternet();
        }
    );
}

void OTPService::verifyOTP(const QString &otp, const QString &email, const QString &auth_type) {
    // Request function
    auto requestFunction = [this, email, otp, auth_type]() {
        // Data preparation
        QJsonObject obj { {"email", email}, {"otp", otp} };

        if (!auth_type.isEmpty()) 
            obj["auth_type"] = auth_type;

        QJsonDocument doc(obj);

        QNetworkReply *reply = manager->post(
            configureNetworkRequest(route(APIRoutes::OTP_VERIFY)), 
            doc.toJson()
        );

        connect(reply, &QNetworkReply::finished, this, [this, reply, auth_type](){
            // Network error
            if (reply->error() != QNetworkReply::NoError) {
                ERROR_HERE("Network request failed:  " + reply->errorString());
                emit somethingWentWrong(OTPAction::Verify);
                reply->deleteLater();
                return;
            }

            // Response from server
            QByteArray response = reply->readAll();
            QJsonDocument r = QJsonDocument::fromJson(response);

            if (!r.isObject()) {
                reply->deleteLater();
                return;
            }

            // Main Object
            QJsonObject mainObj = r.object();
            qDebug() << mainObj;
            
            // Server Errors
            int statusCode = mainObj["status_code"].toInt();
            if (statusCode > 0) {
                switch (statusCode) {
                    case 401:
                    case 500:    somethingWentWrong(OTPAction::Verify);      return;
                    case 403:    maxLimitReached();                          return;
                }
            }

            // OTP verification success
            bool isVerified = mainObj["isVerified"].toBool();
            if (!auth_type.isEmpty()) 
                emit OTPVerified(isVerified, mainObj);
            else 
                emit OTPVerified(isVerified);

            INFO_HERE("OTP verification status is : " + QString(isVerified ? "True" : "False"));
            reply->deleteLater();
        });
    };

    Utils::InternetConnectivity::instance().runIfOnline(
        [this, requestFunction](){
            requestFunction();
        },
        this,
        "Auth",
        [this](){
            emit noInternet();
        }
    );
}

QNetworkRequest OTPService::configureNetworkRequest(const QString &route) {
    QUrl url(route);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("api_key", API_KEY.toUtf8());
    request.setRawHeader("accept", "application/json");
    request.setTransferTimeout(REQUEST_TIMEOUT);
    return request;
}
