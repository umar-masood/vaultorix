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

#include "../../../../ui/account/accountCreate/AccountCreate.h"
#include "../validatorUtils/ValidatorUtils.h"

/* ---------------------------   Email Validator -------------------------  */
class EmailValidator : public QObject {
    Q_OBJECT

    public:
    explicit EmailValidator(QObject *parent = nullptr);
    bool isValidEmail(QByteArray &email);
    void isEmailAvailable(QByteArray &email);

    private:
    // Validator Utils
    ValidatorUtils *vu = nullptr;

    // Network Manager
    QNetworkAccessManager *manager = nullptr;

    // API Response
    int statusCode;
    QString message;

    // Caching Data Structures
    std::unordered_set<std::string> tempMails;
    std::list<std::string> order;
    std::unordered_map<std::string, std::list<std::string>::iterator> cacheMap;

    // Max Size of Cache
    const size_t MAX_CACHE_SIZE = 5000;

    // Helper Methods
    std::string getFilePath() const;
    bool isEmailBlacklisted(const std::string &domain);
    void loadMailsFromFile();

    signals:
    void emailAvailable(bool isAvailable);
    void unableToCheckEmailAvailability();
};

/*  -----------------------------  Get Email ------------------------------- */
class GetEmail : public QObject {
    Q_OBJECT

    public:
    explicit GetEmail(QObject *parent = nullptr);
    void setAccountCreateObject(AccountCreate *ac = nullptr);

    private:
    // Timer 
    QTimer *timer = nullptr;

    // Email Validator
    EmailValidator *emailValidator = nullptr;

    // Account Create
    AccountCreate *ac = nullptr;

    // Attempts Tracker
    int retryAttempts = 0;

    // Stores Email Field Text
    QByteArray text;

    // Slots
    void onEmailChanged(const QString &pwd);
    void onEmailAvailable(bool isAvailable);    
    void onUnableToCheckEmailAvailability();
    void onTimeout();

    signals:
    void emailValidated(bool isValid);
};