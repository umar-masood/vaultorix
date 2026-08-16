#pragma once

#include <QObject>
#include <unordered_set>
#include <unordered_map>
#include <list>

namespace Utils { class BlacklistManager; }

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

    private slots:
    bool isPasswordBlacklisted(const std::string &password);
    void loadPwdsFromFile();

    private:
    Utils::BlacklistManager *blacklistManager = nullptr;

    std::unordered_set<std::string> weakPwds;
    std::list<std::string> order;
    std::unordered_map<
        std::string,
        std::list<std::string>::iterator
    > cacheMap;

    static constexpr size_t MAX_CACHE_SIZE = 1000;
};
