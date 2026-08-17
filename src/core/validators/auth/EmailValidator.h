#pragma once

#include <QSet>
#include <unordered_map>
#include <list>
#include "../../utils/Utils.h"

class EmailValidator : public QObject {
    Q_OBJECT

    public:
    explicit EmailValidator(QObject *parent = nullptr);
    void checkEmailValidityAndAvailability(const QString &email);

    private:
    // Blacklist Manager
    Utils::BlacklistManager *_blacklistManager = nullptr;

    // Network Manager
    QNetworkAccessManager *_manager = nullptr;

    // LRU
    QSet<QString> _blacklistedDomains;
    std::list<QString> _order;
    std::unordered_map<QString, std::list<QString>::iterator> _cacheMap;

    // Max Size of Cache
    const size_t MAX_CACHE_SIZE = 5000;

    // Helpers
    bool isDomainBlacklisted(const QString &domain);
    bool isValidEmail(const QString &email);
    void loadDomainsFromFile();

    signals:
    void emailAvailable(bool isAvailable);
    void failedToValidateEmail();
    void emailInvalid();
};