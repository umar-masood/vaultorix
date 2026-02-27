#pragma once

#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <list>

#include "../../../ui/auth/signup/Signup.h"
#include "../../utils/ValidatorUtils.h"

class PasswordValidator : public QObject{
    public:
    explicit PasswordValidator(QObject *parent = nullptr);
    bool isValidPwd(QByteArray &pwdBytes, PasswordRules *pwdRules);

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
    void setAccountSignup(Signup *instance = nullptr);

    private:
    // Timer to delay checking of password while user typing
    QTimer *timer = nullptr;

    // Password Validator
    PasswordValidator *pwdValidate = nullptr;

    // Current Account Create
    Signup *ac = nullptr;

    // Slot
    void onPwdChanged(const QString &pwd);

    signals:
    void pwdValidated(bool isValid);
};