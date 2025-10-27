#include "emailValidator.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QStringList>
#include <QRegularExpression>
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

bool EmailValidator::isValidEmail(const std::string &email) {
    QString str = QString::fromStdString(email).trimmed();

    if (str.isEmpty() || !str.contains('@'))
        return false;

    if (str.count('@') != 1)
        return false;

    if (str.contains(' ') || str.contains('\t'))
        return false;

    QStringList parts = str.split('@');
    if (parts.size() != 2)
        return false;

    QString local = parts[0];
    QString domain = parts[1];

    if (local.isEmpty() || domain.isEmpty())
        return false;

    if (local.startsWith('.') || local.endsWith('.') || local.contains(".."))
        return false;

    if (domain.startsWith('.') || domain.endsWith('.') || domain.contains(".."))
        return false;

    if (!domain.contains('.'))
        return false;

    for (QChar c : str) {
        if (c.unicode() > 127)
            return false;
    }

    static const QRegularExpression localRe("^[A-Za-z0-9._%+-]+$");
    static const QRegularExpression domainRe("^(?:[A-Za-z0-9-]+\\.)+[A-Za-z]{2,24}$");

    if (!localRe.match(local).hasMatch())
        return false;

    if (!domainRe.match(domain).hasMatch())
        return false;

    QStringList labels = domain.split('.');
    for (const QString &label : labels) {
        if (label.startsWith('-') || label.endsWith('-') || label.isEmpty())
            return false;
    }

    return true;
}

bool EmailValidator::checkDisposableEmail(const std::string &email) {
    if (!isValidEmail(email)) return true;

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
