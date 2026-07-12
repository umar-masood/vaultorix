#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QChar>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

namespace Core {
    class OTPService : public QObject {
        Q_OBJECT

        public:
        enum class OTPAction { Send, Verify };
        explicit OTPService(QObject *parent = nullptr);
        void sendOTP(const QString &email);
        void verifyOTP(const QString &otp, const QString &email, const QString &auth_type = QString());

        signals:
        void maxLimitReached();
        void noInternet();
        void OTPSent();
        void OTPVerified(bool isVerified, const QJsonObject &obj = QJsonObject());
        void somethingWentWrong(OTPAction action);

        private:
        // Network Manager
        QNetworkAccessManager *manager = nullptr;
        QNetworkRequest configureNetworkRequest(const QString &route);

        // API Credentials
        const QString API_KEY = "API_KEY";
    };
};