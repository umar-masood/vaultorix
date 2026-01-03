#pragma once

#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <list>

#include "../../../ui/pwdRulesWidget/PwdRulesWidget.h"
#include "../../../ui/accountCreate/AccountCreate.h"
#include "../validatorUtils/validatorUtils.h"

class PwdValidator : public QObject{

    public:
    explicit PwdValidator(QObject *parent = nullptr);
    bool isValidPwd(QByteArray &pwdBytes, PwdRulesWidget *pwdRules);

    private:
    // Validator Utils
    ValidatorUtils *vu = nullptr;
    
    // Caching Data Structures
    std::unordered_set<std::string> weakPwds;
    std::list<std::string> order;
    std::unordered_map<std::string, std::list<std::string>::iterator> cacheMap;

    // Cache Max Size
    const size_t MAX_CACHE_SIZE = 1000;

    // Helper Methods
    bool isPwdBlacklisted(const std::string &password);
    void loadPwdsFromFile();
};

class GetPassword : public QObject {
    Q_OBJECT

    public:
    explicit GetPassword(QObject *parent = nullptr);
    void setAccountCreateObject(AccountCreate *ac = nullptr);

    private:
    // Timer to delay checking of password while user typing
    QTimer *timer = nullptr;

    // Password Validator
    PwdValidator *pwdValidate = nullptr;

    // Current Account Create
    AccountCreate *ac = nullptr;

    // Slot
    void onPwdChanged(const QString &pwd);

    signals:
    void pwdValidated(bool isValid);
};