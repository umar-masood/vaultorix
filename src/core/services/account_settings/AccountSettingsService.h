#pragma once

#include <QObject>
#include <string>
#include <QFile>
#include <QFileInfo>
#include <QSettings>

namespace Core {
    class AccountSettingsService : public QObject {
        Q_OBJECT

        public:
        explicit AccountSettingsService(QObject *parent = nullptr);
        enum Error {
            ServerError,
            NoInternet,
            Other
        };

        void setNew2FAStatus(bool has2FAEnabled);
        void setProfilePicturePath(const QString &filePath);
        void setUsername(const QString &username);
        void setPassword(const QString &old_pass, const QString &new_pass);
        void deleteProfilePicture();
        void deleteAccount();
        void updateLockTimeout(int index);
        int fetchLockTimeout();

        private:
        bool isValidResponse(const QJsonObject &obj);
        bool loadProfilePicture();
        void updateProfilePicture();
        void updateUsername();
        void update2FAStatus();
        bool is2FAStatusSame();

        QSettings settings;
        bool new2FAStatus;
        QString profilePicturePath, _username;
        QFileInfo fileInfo;
        QByteArray _profilePictureData;

        signals:
        void invalidActivity();
        void usernameUpdated();
        void profilePictureUpdated();
        void accountDeleted();
        void passwordUpdated();
        void profilePictureDeleted();
    
        void failedToDeleteProfilePicture(Error failure);
        void failedToUpdatePassword(Error failure);
        void failedToDeleteAccount(Error failure);        
        void failedToUpdateProfilePicture(Error failure);
        void failedToUpdateUsername(Error failure);
    };
};