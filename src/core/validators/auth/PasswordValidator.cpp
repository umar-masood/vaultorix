
#include "PasswordValidator.h"
#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../utils/Utils.h"
#include <QDebug>
#include <QUrl>
#include <fstream>

PasswordValidator::PasswordValidator(QObject *parent) : QObject(parent) {
    blacklistManager = new Utils::BlacklistManager(this, "Password");
    blacklistManager->setFileName(PWD_BLACKLIST_FILE);

    if (blacklistManager->downloadList(QUrl(QString::fromUtf8(APIRoutes::PWD_BLACKLIST))))
        INFO_HERE("Password blacklist is being downloaded.");
    else
        loadPwdsFromFile();

    connect(blacklistManager, &Utils::BlacklistManager::listDownloaded, this, [this]() {
        INFO_HERE("Password blacklist has been downloaded.");
        loadPwdsFromFile();
    });
}

void PasswordValidator::checkPasswordValidity(const QString &password) {
    PasswordValidationResult result;

    result.hasLength = password.size() >= 8;

    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    for (QChar ch : password) {
        ushort c = ch.unicode();

        if (c >= 'A' && c <= 'Z')
            hasUpper = true;
        else if (c >= 'a' && c <= 'z')
            hasLower = true;
        else if (c >= '0' && c <= '9')
            hasDigit = true;
        else
            hasSpecial = true;
    }   

    result.hasUpper = hasUpper;
    result.hasLower = hasLower;
    result.hasDigit = hasDigit;
    result.hasSpecial = hasSpecial;

    QString lowerPwd = password.toLower();
    std::string pwdStd = lowerPwd.toStdString();

    if (weakPwds.empty()) {
        result.notWeak = false;
        result.isStrong = false;

        emit validationUpdated(result);

        ERROR_HERE("Weak passwords list is not loaded.");
        return;
    }

    result.notWeak = !isPasswordBlacklisted(pwdStd);

    result.isStrong = result.hasLength  &&
                      result.hasUpper   &&
                      result.hasLower   &&
                      result.hasDigit   &&
                      result.hasSpecial &&
                      result.notWeak;

    emit validationUpdated(result);
}

bool PasswordValidator::isPasswordBlacklisted(const std::string &password) {
    auto it = cacheMap.find(password);

    if (it != cacheMap.end()) {
        order.splice(order.end(), order, it->second);
        return true;
    }

    bool result = weakPwds.find(password) != weakPwds.end();

    if (result) {
        order.push_back(password);
        cacheMap[password] = std::prev(order.end());
        if (cacheMap.size() > MAX_CACHE_SIZE) {
            std::string oldestPwd = order.front();
            cacheMap.erase(oldestPwd);
            order.pop_front();
        }
    }

    return result;
}

void PasswordValidator::loadPwdsFromFile() {
    std::ifstream file(blacklistManager->filePath);

    if (!file.is_open()) {
        ERROR_HERE("Could not open file.");
        return;
    }

    weakPwds.reserve(131554);

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            line.pop_back();

        Utils::lower(line);

        if (!line.empty())
            weakPwds.insert(line);
    }

    file.close();

    INFO_HERE("Loaded " + QString::number(weakPwds.size()) + " passwords.");
}