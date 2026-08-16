#include "EmailValidator.h"
#include "../../config/APIConfig.h"
#include "../../config/Constants.h"

EmailValidator::EmailValidator(QObject *parent) : QObject(parent) {
    blacklistManager = new Utils::BlacklistManager(this, "Email");
    manager = new QNetworkAccessManager(this);
    
    blacklistManager->setFileName(EMAIL_BLACKLIST_FILE);
    if (blacklistManager->downloadList(QUrl(QString::fromUtf8(APIRoutes::EMAIL_BLACKLIST)))) 
        INFO_HERE("Email blacklist is being downloaded.");
    else
        loadMailsFromFile();

    connect(blacklistManager, &Utils::BlacklistManager::listDownloaded, this, [this]() {
        INFO_HERE("Email blacklist has been downloaded.");
        loadMailsFromFile();
    });
}

bool EmailValidator::isValidEmail(const QString &email) {
    // Basic email validation checks
    if (email.isEmpty() || !email.contains('@')) 
        return false;

    if (email.count('@') != 1)
        return false;

    if (email.contains(' ') || email.contains('\t'))
        return false;
    
    if (email.size() > 254)
        return false;

    // Split email into local and domain parts
    int atIndex = email.indexOf('@');
    QString local = email.left(atIndex);
    QString domain = email.mid(atIndex + 1);

    if (local.isEmpty() || domain.isEmpty())
        return false;

    if (local.startsWith('.') || local.endsWith('.') || local.contains(".."))
        return false;

    if (domain.startsWith('.') || domain.endsWith('.') || domain.contains(".."))
        return false;

    if (!domain.contains('.'))
        return false;

    // Check all characters are in valid range - ASCII
    for (QChar c : email) 
        if (c.unicode() > 127)
            return false;

    // Regex patterns for local and domain
    static const QRegularExpression localRe("^[A-Za-z0-9._%+-]+$");
    static const QRegularExpression domainRe("^(?:[A-Za-z0-9-]+\\.)+[A-Za-z]{2,24}$");

    if (!localRe.match(local).hasMatch())
        return false;

    if (!domainRe.match(domain).hasMatch())
        return false;

    // Check individual domain labels
    QList<QString> labels = domain.split('.');
    for (const QString &label : labels) {
        if (label.startsWith('-') || label.endsWith('-') || label.isEmpty())
            return false;
    }

    // Check if email is disposable
    if (tempMails.empty()) {
        ERROR_HERE("Weak email lists cannot be loaded.");
        return false; // If disposable email list is not loaded, we cannot check
    }

    // Extract domain part
    std::string domainStr = domain.toStdString();
    Utils::lower(domainStr);

    // Check if the domain is disposable
    bool isDisposable = isEmailBlacklisted(domainStr);

    // Securely wipe temporary std::string and QByteArray
    Utils::cleanupMemory(domainStr);

    // Return true if email is valid and not disposable
    return !isDisposable;
}

bool EmailValidator::isEmailBlacklisted(const std::string &domain) {
    auto it = cacheMap.find(domain);
    if (it != cacheMap.end()) {
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

void EmailValidator::checkEmailValidityAndAvailability(const QString &email) {
    if (!isValidEmail(email)) {
        emit emailInvalid();
        return;
    }

    QUrl url(route(APIRoutes::CHK_EMAIL) + email);

    QNetworkRequest request(url);
    request.setTransferTimeout(REQUEST_TIMEOUT);
    
    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply, &email](){
        if (!reply)
            return;

        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            ERROR_HERE("Network request failed:  " + reply->errorString());
            emit failedToCheckEmail();
            return;
        }

        QByteArray data = reply->readAll();
        reply->deleteLater();

        if (data.isEmpty())
            return;

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject())
            return;

        QJsonObject obj = jsonDoc.object();
        int statusCode = obj["status_code"].toInt();
        DEBUG_HERE(QString::number(statusCode) + "   " + obj["message"].toString());
        
        emit emailAvailable(statusCode == 200);
    });
}

void EmailValidator::loadMailsFromFile() {
    std::ifstream file(blacklistManager->filePath);
    if (!file.is_open()) {
        ERROR_HERE("Email domains list does not open.");
        return;
    }

    tempMails.reserve(71500);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            line.pop_back();

        Utils::lower(line);

        if (!line.empty())
            tempMails.insert(line);
    }

    file.close();

    INFO_HERE("Loaded " + QString::number(tempMails.size()) + " domains.");
}
