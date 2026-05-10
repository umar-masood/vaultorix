#include "SignupService.h"

#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../utils/Utils.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonObject>
#include <QJsonParseError>

using Core::SignupService;
SignupService::SignupService(QObject *parent) : QObject(parent) {
    // Network Manager
    manager = new QNetworkAccessManager(this);

    // Generate device id and stores it if it does not exists
    storeDeviceId();
}

// Check validation status for enable/disable create account button
void SignupService::checkValidationStatus() {
    for (auto it = validationStatus.begin(); it != validationStatus.end(); ++it) {
        if (!it->second) {
            emit validationDone(false);
            return;
        }
    }
    emit validationDone(true);
}

// Preparing json data for request
QJsonDocument SignupService::prepareRequestData(const SignupData &data) {
    QJsonObject userObj {
        {"full_name",   data.fullName},
        {"username",    data.username},
        {"email",       data.email},
        {"password",    data.password},
        {"created_at",  QDateTime::currentDateTimeUtc().toString(Qt::ISODate)},
        {"is_active",   true}
    };

    QJsonObject deviceObj {
        {"device_name",     deviceInfo.getDeviceName()},
        {"device_id",       getDeviceIdLocally()},
        {"os_type",         deviceInfo.getOSName()},
        {"os_version",      deviceInfo.getProductVersion()},
        {"kernel_version",  deviceInfo.getKernelVersion()}
    };

    QJsonObject appObj {
        {"version", QCoreApplication::applicationVersion()}
    };

    QJsonObject mainObj {
        {"user",        userObj},     
        {"device_info", deviceObj},
        {"app_info",    appObj}
    };

    return QJsonDocument(mainObj);
}

// Calling api for storing user credentials
void SignupService::storeCredentials(const SignupData &data) {
    auto requestFunction = [this, data]() {
        QNetworkRequest request(QUrl(route(APIRoutes::SIGNUP)));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Accept", "application/json");
        request.setRawHeader("api_key", API_KEY.toUtf8());
        request.setTransferTimeout(REQUEST_TIMEOUT);

        QNetworkReply *reply = manager->post(request, prepareRequestData(data).toJson());

        connect(reply, &QNetworkReply::finished, this, [this, reply, data]() {
            if (reply->error() != QNetworkReply::NoError) {
                reply->deleteLater();
                ERROR_HERE("Network request failed:  " + reply->errorString());
                emit failedToStoreCredentials(Error::SomethingWentWrong);
                return;
            }

            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
            reply->deleteLater();

            if (parseError.error != QJsonParseError::NoError || !doc.isObject())
                return;

            QJsonObject obj = doc.object();
            int statusCode = obj["status_code"].toInt();
            DEBUG_HERE(QString::number(statusCode) + "  " + obj["message"].toString());

            switch (statusCode) {
                case 200:   emit credentialsStored(data.email);                                 break;
                case 403:   emit failedToStoreCredentials(Error::MaxAttemptsLimitReached);      break;
                default:    emit failedToStoreCredentials(Error::SomethingWentWrong);           break;
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

// Store device unique identifier
void SignupService::storeDeviceId() {
    if (!settings.contains(DEVICE_ID_SETTINGS_KEY))
        settings.setValue(DEVICE_ID_SETTINGS_KEY, deviceInfo.getDeviceId());
}

// Get device unique identifier
QString SignupService::getDeviceIdLocally() const {
    return settings.value(DEVICE_ID_SETTINGS_KEY).toString();
}

void SignupService::setUsernameValidity(bool isValid) {
    validationStatus["username"] = isValid;
    checkValidationStatus();
}

void SignupService::setEmailValidity(bool isValid) {
    validationStatus["email"] = isValid;
    checkValidationStatus();
}

void SignupService::setPasswordValidity(bool isValid) {
    validationStatus["password"] = isValid;
    checkValidationStatus();
}

void SignupService::setNameValidity(bool isValid) {
    validationStatus["fullName"] = isValid;
    checkValidationStatus();
}

void SignupService::setTCValidity(bool isValid) {
    validationStatus["acceptedTC"] = isValid;
    checkValidationStatus();
}