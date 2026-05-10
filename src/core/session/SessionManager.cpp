#include "SessionManager.h"
#include <QJsonObject>
#include "../config/Constants.h"
#include "../services/auth/TokenManager.h"

SessionManager::SessionManager(QObject* parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

SessionManager& SessionManager::instance() {
    static SessionManager instance;
    return instance;
}

void SessionManager::loadFromJson(const QJsonObject &mainObj) {
    if (mainObj.isEmpty())
        return;

    QJsonObject userObj = mainObj["user"].toObject();

    setFullName(userObj.value("full_name").toString());
    setEmail(userObj.value("email_address").toString());
    setUsername(userObj.value("username").toString());
    set2FAStatus(userObj.value("is_two_factor_enabled").toBool());
    setUsernameChangePermission(userObj.value("allowed_to_change_username").toBool());
    setAvatarUrl(userObj.value("avatar_url").toString());

    Core::TokenManager::instance()->extractTokens(mainObj);

    emit userInfoChanged();
}

void SessionManager::setUsername(const QString &username) {
    _username = username; 
}

void SessionManager::setFullName(const QString &fullName) {
    _fullName = fullName;
}

void SessionManager::set2FAStatus(bool isEnabled) {
    is2FAEnabled = isEnabled;
}

void SessionManager::setUsernameChangePermission(bool isAllowed) {
    isAllowedToChangeUsername = isAllowed;
}

void SessionManager::setAvatarUrl(const QString &url) {
    if (url.isEmpty())
        return;
    
    _avatarUrl = url;
    downloadAvatar();
}

void SessionManager::setAvatarData(const QByteArray &data) {
    _avatarData = data;

    if (!_avatarData.isEmpty())
        _avatarPixmap.loadFromData(_avatarData);
    else
        _avatarPixmap = QPixmap();        
        
    emit avatorUpdated();
}

void SessionManager::setEmail(const QString &email) {
    _email = email;
}   

void SessionManager::downloadAvatar() {
    QNetworkRequest request(_avatarUrl);
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            ERROR_HERE("Failed to download avatar");
            return;
        }

        _avatarData = reply->readAll();
        
        if (!_avatarData.isEmpty())
            _avatarPixmap.loadFromData(_avatarData);
        else
            _avatarPixmap = QPixmap();

        emit avatorUpdated();
        reply->deleteLater();
    });
}