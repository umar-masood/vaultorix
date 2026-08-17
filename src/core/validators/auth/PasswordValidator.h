#pragma once

#include <QObject>
#include <QSet>
#include <unordered_map>
#include <list>
#include "../../utils/Utils.h"

class PasswordValidator : public QObject {
    Q_OBJECT

    public:
    struct PasswordValidationResult {
        bool hasLength = false;
        bool hasUpper = false;
        bool hasLower = false;
        bool hasDigit = false;
        bool hasSpecial = false;
        bool notWeak = false;
        bool isStrong = false;
    };

    explicit PasswordValidator(QObject *parent = nullptr);
    void checkPasswordValidity(const QString &password);

    signals:
    void validationUpdated(const PasswordValidationResult &result);

    private:
    Utils::BlacklistManager *_blacklistManager = nullptr;

    QSet<QString> _blacklistedPasswords;
    std::list<QString> _order;
    std::unordered_map<QString, std::list<QString>::iterator> _cacheMap;

    const size_t MAX_CACHE_SIZE = 1000;

    // Helpers
    bool isPasswordBlacklisted(const QString &password);
    void loadBlacklistedPwdsFromFile();
};
