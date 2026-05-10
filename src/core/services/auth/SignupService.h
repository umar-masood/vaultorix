#pragma once
#include <QObject>
#include <QSettings>
#include <QJsonDocument>
#include <QNetworkAccessManager>

#include "../../system/DeviceInfo.h"

namespace Core {
    class SignupService : public QObject {
        Q_OBJECT

        public:
        struct SignupData {
            QString fullName, username, email, password;  
        };

        enum class Error {
            SomethingWentWrong,
            MaxAttemptsLimitReached,
        };

        explicit SignupService( QObject *parent = nullptr);
        void storeCredentials(const SignupData &data);
        void setNameValidity(bool isValid);
        void setUsernameValidity(bool isValid);
        void setEmailValidity(bool isValid);
        void setPasswordValidity(bool isValid);
        void setTCValidity(bool isValid);

        private:
        std::unordered_map<std::string, bool> validationStatus = {
            {"username",    false},
            {"email",       false},
            {"password",    false},
            {"fullName",    false},
            {"acceptedTC",  false}
        };

        // Network Manager
        QNetworkAccessManager *manager = nullptr;

        // API Credentials
        const QString API_KEY = "hzza20j1cAS0vn74ioi3zjerwqsabn45556";

        // Device Information
        DeviceInfo deviceInfo;

        // Storing settings
        QSettings settings;
        
        // Helpers
        void checkValidationStatus();
        QJsonDocument prepareRequestData(const SignupData &data);

        void storeDeviceId();
        QString getDeviceIdLocally() const;

        signals:
        void validationDone(bool isDone);
        void credentialsStored(const QString &email);
        void failedToStoreCredentials(const Error &error);
    };
};