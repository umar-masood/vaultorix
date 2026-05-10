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

        private:
        bool isValidResponse(const QJsonObject & obj);

        /**
         * This method loads and copy the profile picture to app directory
         */
        bool loadProfilePicture();

        /**
         * This method will make a request to server to update profile picture
         */
        void updateProfilePicture();

        /**
         * This method makes a request to server to update the username with new one
         */
        void updateUsername();

        /**
         * This method sends a request to update the status of 2FA to the server
         */
        void update2FAStatus();

        /**
         * Checks whether the current value and previous value of 2FA is same?
         */
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