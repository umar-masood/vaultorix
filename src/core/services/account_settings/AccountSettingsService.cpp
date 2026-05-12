#include "AccountSettingsService.h"
#include "../../config/Constants.h"
#include "../../config/APIConfig.h"
#include "../auth/TokenManager.h"
#include "../../utils/Utils.h"
#include "../../session/SessionManager.h"

#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QHttpMultiPart>
#include <QHttpPart>

using Core::AccountSettingsService;
using Core::TokenManager;
using Utils::InternetConnectivity;

AccountSettingsService::AccountSettingsService(QObject *parent) : QObject(parent) {}

void AccountSettingsService::setNew2FAStatus(bool has2FAEnabled) {
    new2FAStatus = has2FAEnabled;
    if (is2FAStatusSame()) {
        WARN_HERE("Your previous and current 2FA status is same.");
        return;
    } else {
        INFO_HERE("We are updaing your 2FA status.");
        update2FAStatus();
    }
}

void AccountSettingsService::setProfilePicturePath(const QString &filePath) {
    profilePicturePath = filePath;
    
    if (!loadProfilePicture())
        return;

    updateProfilePicture();
}

void AccountSettingsService::setUsername(const QString &username) {
    if (username.isEmpty())
        return;

    _username = username;

    updateUsername();
}

void AccountSettingsService::setPassword(const QString &old_pass, const QString &new_pass) {
    if (old_pass.isEmpty() || new_pass.isEmpty()) 
        return;

    auto responseCallable = [this](const QJsonObject &obj) {
        if (!isValidResponse(obj)) {
            DEBUG_HERE("Invalid JSON Response structure.");
            emit failedToUpdatePassword(Error::Other);
            return;
        }

        if (obj["status_code"].toInt() == 200) {
            emit passwordUpdated();
            INFO_HERE("Your password has been updated.");
            return; 
        } else {
            DEBUG_HERE(QString::number(obj["status_code"].toInt()) + "   " + obj["message"].toString());
            emit failedToUpdatePassword(Error::ServerError);
        }
    };

    // Network Request Failure Callable
    auto networkRequestFailureCallable = [this](QNetworkReply *reply) { emit failedToUpdatePassword(Error::Other); };

    // Request Data
    QJsonObject mainObj {
        {"old_password", old_pass },
        {"new_password", new_pass }
    };
    
    QJsonDocument doc(mainObj);

    // POST Request
    InternetConnectivity::instance().runIfOnline(
        [this, responseCallable, networkRequestFailureCallable, doc](){
            TokenManager::instance()->sendRequest(
                route(APIRoutes::UPDATE_PASSWORD),
                doc.toJson(),
                QNetworkAccessManager::PostOperation,
                responseCallable,
                networkRequestFailureCallable
            );
        },
        this,
        "AccountSettings",
        [this](){ emit failedToUpdatePassword(Error::NoInternet); }
    );
}

void AccountSettingsService::deleteProfilePicture() {
    auto responseCallable = [this](const QJsonObject &obj) {
        if (!isValidResponse(obj)) {
            DEBUG_HERE("Invalid JSON Response structure.");
            emit failedToDeleteProfilePicture(Error::Other);
            return;
        }

        if (obj["status_code"].toInt() == 200) {
            emit profilePictureDeleted();
            _profilePictureData.clear();
            SessionManager::instance().setAvatarData(_profilePictureData);
            INFO_HERE("Your account profile picture has been deleted successfully.");
            return; 
        } else {
            DEBUG_HERE(QString::number(obj["status_code"].toInt()) + "   " + obj["message"].toString());
            emit failedToDeleteProfilePicture(Error::ServerError);
        }
    };

    // Network Request Failure Callable
    auto networkRequestFailureCallable = [this](QNetworkReply *reply) { emit failedToDeleteProfilePicture(Error::Other); };

    // POST Request
    InternetConnectivity::instance().runIfOnline(
        [this, responseCallable, networkRequestFailureCallable](){
            TokenManager::instance()->sendRequest(
                route(APIRoutes::DELETE_PROFILE_PICTURE),
                QByteArray(),
                QNetworkAccessManager::DeleteOperation,
                responseCallable,
                networkRequestFailureCallable
            );
        },
        this,
        "AccountSettings",
        [this](){ emit failedToDeleteProfilePicture(Error::NoInternet); }
    );
}

void AccountSettingsService::deleteAccount() {
    auto responseCallable = [this](const QJsonObject &obj) {
        if (!isValidResponse(obj)) {
            DEBUG_HERE("Invalid JSON Response structure.");
            emit failedToDeleteAccount(Error::Other);
            return;
        }
        
        if (obj["status_code"].toInt() == 200) {
            emit accountDeleted();
            INFO_HERE("Your account has been deleted successfully.");
            return; 
        } else {
            DEBUG_HERE(QString::number(obj["status_code"].toInt()) + "   " + obj["message"].toString());
            emit failedToDeleteAccount(Error::ServerError);
        }
    };

    // Network Request Failure Callable
    auto networkRequestFailureCallable = [this](QNetworkReply *reply) { emit failedToDeleteAccount(Error::Other); };

    // POST Request
    InternetConnectivity::instance().runIfOnline(
        [this, responseCallable, networkRequestFailureCallable](){
            TokenManager::instance()->sendRequest(
                route(APIRoutes::DELETE_ACCOUNT),
                QByteArray(),
                QNetworkAccessManager::DeleteOperation,
                responseCallable,
                networkRequestFailureCallable
            );
        },
        this,
        "AccountSettings",
        [this](){ emit failedToDeleteAccount(Error::NoInternet); }
    );
}

bool AccountSettingsService::loadProfilePicture() {
    if (profilePicturePath.isEmpty())
        return false;

    QFileInfo info(profilePicturePath);

    QFile file (profilePicturePath);
    if (!file.open(QIODevice::ReadOnly)) {
        ERROR_HERE("Unable to open the profile picture from provided path");
        return false;
    }

    fileInfo = info;
    _profilePictureData = file.readAll();

    return true;
}

void AccountSettingsService::updateProfilePicture() {
    auto responseCallable = [this](const QJsonObject &obj){
        if (!isValidResponse(obj)) {
            DEBUG_HERE("Invalid JSON response structure.");
            emit failedToUpdateProfilePicture(Error::Other);
            return;
        }

        if (obj["status_code"].toInt() == 200) {
            emit profilePictureUpdated();
            SessionManager::instance().setAvatarData(_profilePictureData);
            INFO_HERE("Your profile picture has been updated.");
        } else {
            DEBUG_HERE(QString::number(obj["status_code"].toInt()) + "   " + obj["message"].toString());
            emit failedToUpdateProfilePicture(Error::ServerError);
        }
    };

    auto networkRequestFailureCallable = [this](QNetworkReply *) { emit failedToUpdateProfilePicture(Error::Other); };

    auto multipartCallable = [this]() {
        QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        QString fileType = fileInfo.suffix().toLower();
        QString mimeType;

        if (fileType == "png")
            mimeType = "image/png";
        else if (fileType == "jpg" || fileType == "jpeg")
            mimeType = "image/jpeg";
        else
            mimeType = "application/octet-stream";

        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, mimeType);
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QVariant(QString("form-data; name=\"picture\"; filename=\"%1\"").arg(fileInfo.fileName())));

        filePart.setBody(_profilePictureData);

        multipart->append(filePart);
        return multipart;
    };

    InternetConnectivity::instance().runIfOnline(
        [this, multipartCallable, responseCallable, networkRequestFailureCallable](){
            TokenManager::instance()->sendMultipartRequest(
                route(APIRoutes::UPDATE_PROFILE_PICTURE),
                multipartCallable,
                QNetworkAccessManager::PostOperation,
                responseCallable,
                networkRequestFailureCallable
            );
        },
        this,
        "AccountSettings",
        [this]() { emit failedToUpdateProfilePicture(Error::NoInternet); }
    );
}

void AccountSettingsService::updateUsername() {
    // Response Callable
    auto responseCallable = [this](const QJsonObject &obj){
        if (!isValidResponse(obj)) {
            DEBUG_HERE("Invalid JSON response structure.");
            emit failedToUpdateUsername(Error::Other);
            return;
        }

        if (obj["status_code"].toInt() == 200) {
            emit usernameUpdated();
            SessionManager::instance().setUsername(_username);
            SessionManager::instance().setUsernameChangePermission(false);
            INFO_HERE("Your username has been updated.");
            return; 
        } else {
            DEBUG_HERE(QString::number(obj["status_code"].toInt()) + "   " + obj["message"].toString());
            emit failedToUpdateUsername(Error::ServerError);
        }
    };

    // Network Request Failure Callable
    auto networkRequestFailureCallable = [this](QNetworkReply *reply) { emit failedToUpdateUsername(Error::Other); };

    // POST Request
    InternetConnectivity::instance().runIfOnline(
        [this, responseCallable, networkRequestFailureCallable](){
            TokenManager::instance()->sendRequest(
                route(APIRoutes::UPDATE_USERNAME) + _username,
                QByteArray(),
                QNetworkAccessManager::PutOperation,
                responseCallable,
                networkRequestFailureCallable
            );
        },
        this,
        "AccountSettings",
        [this](){ emit failedToUpdateUsername(Error::NoInternet); }
    );
}

void AccountSettingsService::update2FAStatus() {
    // Response Callable
    auto responseCallable = [this](const QJsonObject &obj){
        if (!isValidResponse(obj)) {
            ERROR_HERE("Reponse JSON structure is not valid.");
            return;
        }

        if (obj["status_code"].toInt() == 200) {
            SessionManager::instance().set2FAStatus(new2FAStatus);
            INFO_HERE("2FA status has been updated.");
            return; 
        } else 
            DEBUG_HERE(QString::number(obj["status_code"].toInt()) + "   " + obj["message"].toString());
        
    };

    // Network Request Failure Callable
    auto networkRequestFailureCallable = [this](QNetworkReply *reply) {
        DEBUG_HERE("Updating 2FA status request is failed due to network request failure.");
    };

    // POST Request
    InternetConnectivity::instance().runIfOnline(
        [this, responseCallable, networkRequestFailureCallable](){
            TokenManager::instance()->sendRequest(
                route(APIRoutes::UPDATE_2FA_STATUS) + (new2FAStatus ? "true" : "false"),
                QByteArray(),
                QNetworkAccessManager::PutOperation,
                responseCallable,
                networkRequestFailureCallable
            );
        },
        this,
        "AccountSettings"
    );
}

bool AccountSettingsService::isValidResponse(const QJsonObject &obj) {
    return obj.contains("status_code") && obj.contains("message");
}

bool AccountSettingsService::is2FAStatusSame() {
    bool curr2FAStatus = SessionManager::instance().twoFAStatus();
    return curr2FAStatus == new2FAStatus;
}

int AccountSettingsService::fetchLockTimeout() {
    if (!settings.contains(LOCKOUT_SETTINGS_KEY)) {
        settings.setValue(LOCKOUT_SETTINGS_KEY, 0);
        return 0;
    }
    
   return (settings.value(LOCKOUT_SETTINGS_KEY).toInt());
}

void AccountSettingsService::updateLockTimeout(int index) {
    if (index == settings.value(LOCKOUT_SETTINGS_KEY).toInt())
        return;
        
    settings.setValue(LOCKOUT_SETTINGS_KEY, index);
    settings.sync();
}