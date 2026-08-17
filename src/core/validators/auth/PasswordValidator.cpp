
#include "PasswordValidator.h"
#include "../../config/APIConfig.h"
#include "../../config/Constants.h"

#include <QUrl>
#include <QFile>
#include <QTextStream>

PasswordValidator::PasswordValidator(QObject *parent) : QObject(parent) {
    _blacklistManager = new Utils::BlacklistManager("Password", this);
    _blacklistManager->setFileName(PWD_BLACKLIST_FILE);

    if (_blacklistManager->downloadList(QUrl(QString::fromUtf8(APIRoutes::PWD_BLACKLIST))))
        INFO_HERE("Password blacklist download started.");
    else
        loadBlacklistedPwdsFromFile();

    connect(_blacklistManager, &Utils::BlacklistManager::listDownloaded, this, [this]() {
        INFO_HERE("Password blacklist has been downloaded.");
        loadBlacklistedPwdsFromFile();
    });
}

void PasswordValidator::checkPasswordValidity(const QString &password) {
    PasswordValidationResult result;

    for (QChar ch : password) {
        ushort c = ch.unicode();

        if (c >= 'A' && c <= 'Z')
            result.hasUpper = true;
        else if (c >= 'a' && c <= 'z')
            result.hasLower = true;
        else if (c >= '0' && c <= '9')
            result.hasDigit = true;
        else
            result.hasSpecial = true;
    }   

    if (_blacklistedPasswords.empty()) {
        result.notWeak = false;
        result.isStrong = false;

        emit validationUpdated(result);

        ERROR_HERE("Weak passwords list was not loaded.");
        return;
    }

    result.hasLength = password.size() >= 8;
    result.notWeak = !isPasswordBlacklisted(password);
    result.isStrong = result.hasLength  &&
                      result.hasUpper   &&
                      result.hasLower   &&
                      result.hasDigit   &&
                      result.hasSpecial &&
                      result.notWeak;

    emit validationUpdated(result);
}

bool PasswordValidator::isPasswordBlacklisted(const QString &password) {
    auto it = _cacheMap.find(password);
    if (it != _cacheMap.end()) {
        _order.splice(_order.end(), _order, it->second);
        return true;
    }

    bool result = _blacklistedPasswords.find(password) != _blacklistedPasswords.end();
    if (result) {
        _order.push_back(password);
        _cacheMap[password] = std::prev(_order.end());

        if (_cacheMap.size() > MAX_CACHE_SIZE) {
            QString oldestPwd = _order.front();
            _cacheMap.erase(oldestPwd);
            _order.pop_front();
        }
    }

    return result;
}

void PasswordValidator::loadBlacklistedPwdsFromFile() {
    QFile file(_blacklistManager->filePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ERROR_HERE("Weak passwords list failed to open.");
        return;
    }

    _blacklistedPasswords.clear();
    _blacklistedPasswords.reserve(131554);

    _order.clear();
    _cacheMap.clear();
    
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString password = stream.readLine().trimmed();
        if (!password.isEmpty())
            _blacklistedPasswords.insert(password);
    }

    file.close();

    INFO_HERE("Loaded " + QString::number(_blacklistedPasswords.size()) + " passwords.");
}