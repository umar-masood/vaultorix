#include "EmailValidator.h"
#include "../../config/APIConfig.h"
#include "../../config/Constants.h"

#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>
#include <QTextStream>

EmailValidator::EmailValidator(QObject *parent) : QObject(parent) {
    _blacklistManager = new Utils::BlacklistManager("Email", this);    
    _blacklistManager->setFileName(EMAIL_BLACKLIST_FILE);

    if (_blacklistManager->downloadList(QUrl(QString::fromUtf8(APIRoutes::EMAIL_BLACKLIST)))) 
        INFO_HERE("Email domains blacklist download started.");
    else
        loadDomainsFromFile();

    connect(_blacklistManager, &Utils::BlacklistManager::listDownloaded, this, [this]() {
        INFO_HERE("Email domains blacklist has been downloaded.");
        loadDomainsFromFile();
    });

    _manager = new QNetworkAccessManager(this);
}

void EmailValidator::checkEmailValidityAndAvailability(const QString &email) {
    if (!isValidEmail(email)) {
        emit emailInvalid();
        return;
    }

    QNetworkRequest request(QUrl(route(APIRoutes::CHK_EMAIL) + email));
    request.setTransferTimeout(REQUEST_TIMEOUT);
    
    QNetworkReply *reply = _manager->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply](){
        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            ERROR_HERE("Network request failed:  " + reply->errorString());
            emit failedToValidateEmail();
            return;
        }

        const QByteArray data = reply->readAll();
        reply->deleteLater();

        if (data.isEmpty()) {
            emit failedToValidateEmail();
            return;
        }

        QJsonParseError parseError;
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
            emit failedToValidateEmail();
            return;
        }

        const QJsonObject obj = jsonDoc.object();
        int statusCode = obj["status_code"].toInt();

        DEBUG_HERE(QString::number(statusCode) + "   " + obj["message"].toString());
        
        emit emailAvailable(statusCode == 200);
    });
}

bool EmailValidator::isValidEmail(const QString &email) {
    if (email.isEmpty()) 
        return false;

    static QRegularExpression regex("^[A-Za-z0-9][A-Za-z0-9._+]*@[A-Za-z0-9]+(-[A-Za-z0-9]+)*(\\.[A-Za-z0-9]+)+$");
    if (!regex.match(email).hasMatch())
        return false;

    // Check if email is disposable
    if (_blacklistedDomains.empty()) {
        ERROR_HERE("Blacklisted emails was not loaded.");
        return false;
    }

    const QStringList parts = email.split('@');
    // Username
    QString username = parts[0].toLower();

    // Checking consecutive dots in username part
    for (auto i = 1; i < username.size(); i++) {
        if (username[i] == '.' && username[i - 1] == '.')
            return false;
    }

    // Domain 
    QString domain = parts[1].toLower();
    
    // Check if the domain is blacklisted
    bool isBlacklisted = isDomainBlacklisted(domain);

    // Return true if email is valid and domain not blacklisted
    return !isBlacklisted;
}

bool EmailValidator::isDomainBlacklisted(const QString &domain) {
    auto it = _cacheMap.find(domain);
    if (it != _cacheMap.end()) {
        _order.splice(_order.end(), _order, it->second); 
        return true;
    }

    bool result = _blacklistedDomains.find(domain) != _blacklistedDomains.end();
    if (result) {
        _order.push_back(domain);
        _cacheMap[domain] = std::prev(_order.end()); 
        
        if (_cacheMap.size() > MAX_CACHE_SIZE) {
            const QString domain = _order.front();
            _cacheMap.erase(domain);
            _order.pop_front();
        }
    }

    return result;
}

void EmailValidator::loadDomainsFromFile() {
    QFile file(_blacklistManager->filePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ERROR_HERE("Email domains list failed to open.");
        return;
    }
    
    _blacklistedDomains.clear();
    _blacklistedDomains.reserve(71500);

    _order.clear();
    _cacheMap.clear();
    
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString domain = stream.readLine().trimmed().toLower();
        if (!domain.isEmpty())
            _blacklistedDomains.insert(domain);
    }

    file.close();

    INFO_HERE("Loaded " + QString::number(_blacklistedDomains.size()) + " domains.");
}
