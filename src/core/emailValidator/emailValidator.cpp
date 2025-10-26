#include "emailValidator.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cctype>

EmailValidator::EmailValidator() {
    if (downloadList()) 
        std::cout << "List downloaded successfully.\n";
    else 
        std::cerr << "Using local file or failed to download.\n";
    
    loadMailsFromFile();
}

bool EmailValidator::checkDisposableEmail(const std::string &email) {
    size_t pos = email.find('@');
    if (pos == std::string::npos) return true;

    std::string domain = email.substr(pos + 1);
    lower(domain);

    return isDisposableEmail(domain);
}

bool EmailValidator::isDisposableEmail(const std::string &domain) {
    auto it = cacheMap.find(domain);
    if (it != cacheMap.end()){
        std::cout << "domain is already checked...";
        order.splice(order.end(), order, it->second); 
        return true;
    }

    bool result = tempMails.find(domain) != tempMails.end();
    if (result) {
        order.push_back(domain);
        cacheMap[domain] = std::prev(order.end()); 
        if (cacheMap.size() > MAX_CACHE_SIZE) {
            std::string oldestPwd = order.front();
            cacheMap.erase(oldestPwd);
            order.pop_front();
        }
    }

    return result;
}

std::string EmailValidator::getFilePath() const {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/config";
    QDir().mkpath(path);
    return path.toStdString() + "/tempMails.config";
}

void EmailValidator::lower(std::string &str) {
    for (auto &c : str) c = std::tolower(static_cast<unsigned char>(c));
}

bool EmailValidator::isOlderList() const {
    if (std::filesystem::exists(getFilePath())) {
        QFileInfo info(QString::fromStdString(getFilePath()));
        QDateTime modified = info.lastModified();
        return modified.daysTo(QDateTime::currentDateTime()) > 1;
    }
    return true;
}

bool EmailValidator::downloadList() {
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

void EmailValidator::loadMailsFromFile() {
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
