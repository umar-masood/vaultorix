#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>
#include <QPixmap>

class SessionManager : public QObject {
    Q_OBJECT

    public:
    static SessionManager& instance();

    void loadFromJson(const QJsonObject &mainObj);
    void setUsername(const QString &username);
    void setFullName(const QString &fullName);
    void set2FAStatus(bool isEnabled);
    void setEmail(const QString &email);
    void setUsernameChangePermission(bool isAllowed);
    void setAvatarUrl(const QString &url);
    void setAvatarData(const QByteArray &data);

    QString username() const { return _username; }
    QString fullName() const { return _fullName; }
    bool twoFAStatus() const { return is2FAEnabled; }
    QString email() const { return _email; }
    bool allowedToChangeUsername() const { return isAllowedToChangeUsername; }
    QPixmap avatar() const { return _avatarPixmap; }

    signals:
    void avatorUpdated();
    void userInfoChanged();
    
    private:
    explicit SessionManager(QObject* parent = nullptr);
    void downloadAvatar();

    QString _username, _fullName, _email, _avatarUrl;
    QByteArray _avatarData;
    QPixmap _avatarPixmap;

    bool is2FAEnabled = false;
    bool isAllowedToChangeUsername = true;

    QNetworkAccessManager *manager = nullptr;
};