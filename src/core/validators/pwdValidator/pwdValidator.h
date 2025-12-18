#pragma once

#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <list>

#include "../../../ui/pwdRulesWidget/pwdRulesWidget.h"
#include "../../../ui/accountCreate/AccountCreate.h"
#include "../validatorUtils/validatorUtils.h"

class PwdValidator : public QObject{
    public:
    explicit PwdValidator(QObject *parent = nullptr);
    bool isValidPwd(QByteArray &pwdBytes, PwdRulesWidget *pwdRules);

    private:
    ValidatorUtils *vu = nullptr;
    
    std::unordered_set<std::string> weakPwds;
    std::list<std::string> order;
    std::unordered_map<std::string, std::list<std::string>::iterator> cacheMap;
    const size_t MAX_CACHE_SIZE = 1000;

    bool isPwdBlacklisted(const std::string &password);
    void loadPwdsFromFile();
};

class GetPassword : public QObject {
    Q_OBJECT
    public:
    explicit GetPassword(QObject *parent = nullptr);
    
    void setAccountCreateObject(AccountCreate *ac = nullptr);

    private:
    QTimer *timer = nullptr;
    PwdValidator *pwdValidate = nullptr;
    AccountCreate *ac = nullptr;

    signals:
    void pwdValidated(bool isValid);

    private slots:
    void onPwdChanged(const QString &pwd);
};