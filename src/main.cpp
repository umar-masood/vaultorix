#include <iostream>

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QTimer>
#include <QString>
#include <QChar>
#include <QObject>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QEventLoop>

class OTPValidator {
    public:
    OTPValidator() {}

    void sendOTP(const QString& fullName, const QString& username, const QString &email) {
        QNetworkRequest request(API_URL + "sendOtp");
        request.setRawHeader("accept", "application/json");
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QJsonObject obj;
        obj["name"] = fullName;
        obj["username"] = username;
        obj["email"] = email;

        QJsonDocument doc(obj);
        QNetworkReply *reply = manager.post(request, doc.toJson());

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        
        if (reply->error() != QNetworkReply::NoError) {
            std::cerr << "Failed to send otp..." << reply->errorString().toStdString();
            return;
        } 

        QByteArray response = reply->readAll();
        QJsonDocument r = QJsonDocument::fromJson(response);

        if (!r.isObject()) 
            std::cerr << "Sending Response is invalid...";

        QJsonObject rObj = r.object();
        status_code = rObj["status_code"].toInt();
        message = rObj["message"].toString();

        qDebug() << status_code << ",  " << message;
    
        reply->deleteLater();
    }

    bool verifyOTP(const QString &otp, const QString &email) {
      QNetworkRequest request(API_URL + "verifyOtp");
        request.setRawHeader("accept", "application/json");
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QJsonObject obj;
        obj["email"] = email;
        obj["otp"] = otp;

        QJsonDocument doc(obj);
        QNetworkReply *reply = manager.post(request, doc.toJson());

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        
        if (reply->error() != QNetworkReply::NoError) {
            std::cerr << "We cannot be able to verify your entered OTP" << reply->errorString().toStdString();
            return false;
        } 

        QByteArray response = reply->readAll();
        QJsonDocument r = QJsonDocument::fromJson(response);

        if (!r.isObject()) {
            std::cerr << "Verification Response is invalid...";
            return false;
        }

        QJsonObject rObj = r.object();
        isVerified = rObj["isVerified"].toBool();
        reply->deleteLater();
        return isVerified;
    }


    private:
    QNetworkAccessManager manager;
    const QString API_URL = "https://www.umarcreations.site/";
    int status_code;
    bool isVerified = false;
    QString message;

    void cleanupMemory(std::string &str) {
        if (str.empty()) return;
        volatile char *p = str.data();
        for (size_t i = 0; i < str.size(); i++) {
            p[i] = 0;
        }
    }
};

int main(int argc, char *agrv[]) {
    QCoreApplication app(argc, agrv);

    OTPValidator ov;
   // ov.sendOTP("Umar Masood", "umarmasood1203", "umarmasood@gmail.com");
    //ov.verifyOTP("EB1HS", "alimasood7281@gmail.com");
    return 0;
}

