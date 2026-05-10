#pragma once

#include <unordered_set>
#include <unordered_map>
#include <list>
#include <fstream>
#include <QStringList>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include "../../utils/Utils.h"

class EmailValidator : public QObject {
    Q_OBJECT

    public:
    explicit EmailValidator(QObject *parent = nullptr);
    void checkEmailValidityAndAvailability(const QString &email);

    private:
    // Blacklist Manager
    Utils::BlacklistManager *blacklistManager = nullptr;

    // Network Manager
    QNetworkAccessManager *manager = nullptr;

    // Caching Data Structures
    std::unordered_set<std::string> tempMails;
    std::list<std::string> order;
    std::unordered_map<std::string, std::list<std::string>::iterator> cacheMap;

    // Max Size of Cache
    const size_t MAX_CACHE_SIZE = 5000;

    // Helpers
    bool isEmailBlacklisted(const std::string &domain);
    void loadMailsFromFile();
    bool isValidEmail(const QString &email);

    signals:
    void emailAvailable(bool isAvailable);
    void failedToCheckEmail();
    void emailInvalid();
};