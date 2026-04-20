#pragma once

#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include "../../utils/Utils.h"

namespace Ui::Auth { class Signup; };
class PasswordRules;

/**
 * This class will check the password passed by the class GetPassword, whether it is valid?
 */
class PasswordValidator : public QObject{
    public:
    explicit PasswordValidator(QObject *parent = nullptr);
    bool isValidPwd(QByteArray &pwdBytes, PasswordRules *pwdRules);

    private:
    // Validator Utils
    Utils::BlacklistManager *blacklistManager = nullptr;
    
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

/**
 * This class will take the password from password text field from Account Sign up widget in Ui and then passed it to the actual password validator class for validation.
 */
class GetPassword : public QObject {
    Q_OBJECT

    public:
    explicit GetPassword(QObject *parent = nullptr);
    void setAccountSignupWidget(Ui::Auth::Signup *instance = nullptr);

    private:
    // Timer to delay checking of password while user typing
    QTimer *timer = nullptr;

    // Password Validator
    PasswordValidator *pwdValidate = nullptr;

    // Current Account Signup Widget
    Ui::Auth::Signup *signupWidget = nullptr;

    // Slot
    void onPwdChanged(const QString &pwd);

    signals:
    void pwdValidated(bool isValid);
};