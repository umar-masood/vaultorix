#include "SigninService.h"

#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../utils/Utils.h"

using Core::SigninService;

SigninService::SigninService(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

void SigninService::verifyCredentials(const QString &username, const QString &password) {
    auto requestFunction = [this, username, password]() {
        QNetworkRequest request(QUrl(route(APIRoutes::SIGNIN)));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Accept", "application/json");
        request.setRawHeader("api_key", API_KEY.toUtf8());
        request.setTransferTimeout(REQUEST_TIMEOUT);

        QJsonObject mainObj {
            {"username", username},
            {"password", password}
        };

        QJsonDocument doc(mainObj);
        QNetworkReply *reply = manager->post(request, doc.toJson());

        connect(reply, &QNetworkReply::finished, [this, reply]() {
            if (!reply) 
                return;

            if (reply->error() != QNetworkReply::NoError) {
                reply->deleteLater();
                emit failedToSignIn(SignInError::SomethingWentWrong);
                return;
            }

            // Server Response
            QByteArray data = reply->readAll();
            reply->deleteLater();

            if (data.isEmpty()) 
                return;

            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
            if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject())
                return;

            QJsonObject obj = jsonDoc.object();
            int statusCode = obj["status_code"].toInt();
            DEBUG_HERE(QString::number(statusCode) + "   " + obj["message"].toString());

            switch (statusCode) {
                case 200:  emit signedIn(obj);                                                  break;
                case 400:  emit failedToSignIn(SignInError::InvalidCredentials);                break;
                case 403:  emit failedToSignIn(SignInError::MaxAttemptsLimitReached);           break;
                case 511:  emit failedToSignIn(SignInError::AccessDenied);                      break;
                case 513:  emit verificationRequired(obj["email"].toString(), "signin");        break;
                default:   emit failedToSignIn(SignInError::SomethingWentWrong);  
            }
        });
    };

    Utils::InternetConnectivity::instance().runIfOnline(
        [this, requestFunction](){    
            requestFunction();
        }, 
        this,
        "Auth"
    );
}