#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <string>

class MailChecker {
public:
    explicit MailChecker() {
        if (downloadList()) 
            std::cout << "List downloaded successfully.\n";
        else 
            std::cerr << "Using local file or failed to download.\n";
        
        loadMailsFromFile();
    }

    bool checkDisposableEmail(std::string email) {
        size_t pos = email.find('@');
        if (pos == std::string::npos) return false;

        std::string domain = email.substr(pos + 1);
        lower(domain);

        return isDisposableEmail(domain);
    }

    bool isDisposableEmail(const std::string &domain) {
        static std::unordered_map<std::string, bool> cache;
        if (cache.find(domain) != cache.end()) {
            std::cout << "Domain is already checked...";
            return cache[domain];
        }

        bool result = false;
        if (tempMails.find(domain) != tempMails.end()) result = true;
        cache[domain] = result;

        return result;
    }

private:
    std::unordered_set<std::string> tempMails;

    std::string getFilePath() {
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/config";
        QDir().mkpath(path);
        return path.toStdString() + "/tempMails.config";
    }

    static void lower(std::string &str) {
        for (auto &c : str) c = std::tolower(static_cast<unsigned char>(c));
    }

    bool isOlderList() {
        if (std::filesystem::exists(getFilePath())) {
            QFileInfo info(QString::fromStdString(getFilePath()));
            QDateTime modified = info.lastModified();
            return modified.daysTo(QDateTime::currentDateTime()) > 1;
        }
        return true;
    }

    bool downloadList() {
        if (!isOlderList()) return true;

        QNetworkAccessManager manager;
        QNetworkRequest request(QUrl("https://raw.githubusercontent.com/umar-masood/Disposable-Emails-List/refs/heads/main/tempMails.conf"));
        QNetworkReply *reply = manager.get(request);

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec(); 

        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            std::cerr << reply->errorString().toStdString();
            return false;
        }

        QByteArray data = reply->readAll();
        reply->deleteLater();

        if (data.isEmpty()) return false;

        QFile file(QString::fromStdString(getFilePath()));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            std::cerr << "Could not write to file.\n";
            return false;
        }

        file.write(data);
        file.close();
        return true;
    }

    void loadMailsFromFile() {
        std::ifstream file(getFilePath());
        if (!file.is_open()) {
            std::cerr << "Could not open mail list file.\n";
            return;
        }

        tempMails.reserve(72000);
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
                line.pop_back();
            lower(line);
            if (!line.empty())
                tempMails.insert(line);
        }
        file.close();
        std::cout << "Loaded " << tempMails.size() << " domains.\n";
    }
};


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    MailChecker mc;
    std::cout << std::boolalpha << mc.checkDisposableEmail("345sasdwqd@zuiquandaohang.xyz") << "\n";

    return 0;
}
