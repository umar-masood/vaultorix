#pragma once

#include <QString>
#include <QTimer>
#include <QDateTime>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <string>

#include "../../ui/accountCreate/AccountCreate.h"

class EmailValidator {
public:
    explicit EmailValidator();

    bool checkDisposableEmail(const std::string &email);
    bool isDisposableEmail(const std::string &domain);

private:
    std::unordered_set<std::string> tempMails;
    std::list<std::string> order;
    std::unordered_map<std::string, std::list<std::string>::iterator> cacheMap;
    const size_t MAX_CACHE_SIZE = 5000;

    std::string getFilePath() const;
    static void lower(std::string &str);
    bool isOlderList() const;
    bool downloadList();
    bool isValidEmail(const std::string &email);
    void loadMailsFromFile();
};

class GetEmail : public QObject {
    Q_OBJECT
    public:
    explicit GetEmail(QObject *parent = nullptr);
    
    void setAccountCreateObject(AccountCreate *ac = nullptr);

    private:
    QTimer *timer = nullptr;
    EmailValidator *emailValidate = nullptr;
    AccountCreate *ac = nullptr;

    void onEmailChanged(const QString &pwd);
};
