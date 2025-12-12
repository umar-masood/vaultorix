#pragma once
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <string>
#include <iostream>
#include <fstream>

#include <QStringList>
#include <QRegularExpression>
#include <QFile>
#include <QString>
#include <QTimer>

#include "../../../ui/accountCreate/AccountCreate.h"
#include "../validatorUtils/validatorUtils.h"

class EmailValidator {
public:
    explicit EmailValidator();

    bool checkDisposableEmail(QByteArray &email);
    bool isDisposableEmail(const std::string &domain);

private:
    ValidatorUtils *vu = nullptr;

    std::unordered_set<std::string> tempMails;
    std::list<std::string> order;
    std::unordered_map<std::string, std::list<std::string>::iterator> cacheMap;
    const size_t MAX_CACHE_SIZE = 5000;

    std::string getFilePath() const;

    bool isValidEmail(QByteArray &email);
    void loadMailsFromFile();
};

class GetEmail : public QObject {
    Q_OBJECT
    public:
    explicit GetEmail(QObject *parent = nullptr);
    
    void setAccountCreateObject(AccountCreate *ac = nullptr);

    private:
    QTimer *timer = nullptr;
    EmailValidator emailValidator;
    AccountCreate *ac = nullptr;

    private slots:
    void onEmailChanged(const QString &pwd);
    void onTimeout();

    signals:
    void emailValidated(bool isValid);
};
