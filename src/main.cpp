#include <iostream>
#include <sodium.h>

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
    OTPValidator() {
        if (sodium_init() < 0) {
            std::cerr << "Crypto System is not ready...";
        }

        QFile htmlFile(":/otp/otp.html");
        if (!htmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "The OTP html file cannot opened..." << std::endl;
        }

        htmlText = QString::fromUtf8(htmlFile.readAll());
        htmlFile.close();
    }

    void generateOTP() {
        std::string OTP;
        for (int i = 0; i < OTP_MAX_LIMIT; i++) {
            unsigned int idx = randombytes_uniform(OTP_LENGTH);
            OTP += OTP_CHARS[idx];
        }
        // Sent to User
        std::cout << "OTP : " << OTP << std::endl;
        prepareOTPMail("Umar Masood", "umarmasood8546@gmail.com", OTP);
        // Hashing
        hashOTP(OTP.c_str());
        // Cleaning
        cleanupMemory(OTP);
    }

    /*
    {
    "sender": {"name": "YourApp", "email": "no-reply@yourapp.com"},
    "to": [{"email": "user@example.com", "name": "User"}],
    "subject": "Welcome to Our App!",
    "htmlContent": "<html><body><h1>Welcome!</h1><p>Thanks for joining us.</p></body></html>"
    }
    */
    void prepareOTPMail(const std::string &username, const std::string &email, std::string &otp){
        htmlText.replace("{{USERNAME}}", QString::fromStdString(username));
        htmlText.replace("{{OTP}}", QString::fromStdString(otp));
        cleanupMemory(otp);

        QJsonObject json;
        
        // Sender Details
        QJsonObject senderObj;
        senderObj["name"] = "Vaultorix";
        senderObj["email"] = "no-reply@umarcreations.site";
        json["sender"] = senderObj;

        // Receiver Details
        QJsonArray recArr;
        QJsonObject recObj;
        recObj["email"] = QString::fromStdString(email);
        recObj["name"] = QString::fromStdString(username);
        recArr.append(recObj);
        json["to"] = recArr;

        // Subject
        json["subject"] = "Your OTP for Vaultorix Account";

        // Content
        json["htmlContent"] = htmlText;

        QJsonDocument doc(json);
        sendOTP(doc);

        json = QJsonObject();
        doc = QJsonDocument();
    }

    void sendOTP(const QJsonDocument &doc) {
        QNetworkAccessManager manager;
        QNetworkRequest request(API_URL);
        request.setRawHeader("accept", "application/json");
        request.setRawHeader("api-key", API_KEY.toUtf8());
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply *reply = manager.post(request, doc.toJson());

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        
        if (reply->error() != QNetworkReply::NoError) {
            std::cerr << "Failed to send otp..." << reply->errorString().toStdString();
            return;
        } else {
            std::cout << "OTP send successfully..." << std::endl;
        }

        reply->deleteLater();
    }

    void hashOTP(const char *otp) {
        bool isHashed = crypto_pwhash_str(hashedOTP,
                                        otp,
                                        strlen(otp),
                                        crypto_pwhash_OPSLIMIT_MODERATE,
                                        crypto_pwhash_MEMLIMIT_MODERATE) == 0;
        
        if (isHashed) 
            std::cerr << "Hashing Done" << std::endl;
        else
            std::cerr << "Failed to Hash" << std::endl;
        
        std::cout << hashedOTP;
    }

    bool verifyOTP(std::string &otp) {
        if (otp.empty() || otp.length() < 5) return false;
        const char *hashed = hashedOTP;
        const char *localOtp = otp.c_str();
        bool isMatched = crypto_pwhash_str_verify(hashed,
                                                localOtp,
                                                strlen(localOtp)) == 0;
        cleanupMemory(otp);
        sodium_memzero(hashedOTP, sizeof(hashedOTP));

        return isMatched;
    }


    private:
    const QString API_KEY = "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";
    const std::string OTP_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const QUrl API_URL = QUrl("https://api.brevo.com/v3/smtp/email");
    const size_t OTP_LENGTH = OTP_CHARS.size();
    const int OTP_MAX_LIMIT = 5;
    char hashedOTP[crypto_pwhash_STRBYTES];
    QString htmlText;


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
    ov.generateOTP();

    return 0;
}


