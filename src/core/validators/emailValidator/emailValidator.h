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

#include "../../../ui/accountCreate/AccountCreate.h"
#include "../validatorUtils/validatorUtils.h"

class EmailValidator : public QObject {
    Q_OBJECT

    public:
    explicit EmailValidator(QObject *parent = nullptr);

    bool isValidEmail(QByteArray &email);
    void isEmailAvailable(QByteArray &email);

    private:
    ValidatorUtils *vu = nullptr;
    QNetworkAccessManager *manager = nullptr;

    std::unordered_set<std::string> tempMails;
    std::list<std::string> order;
    std::unordered_map<std::string, std::list<std::string>::iterator> cacheMap;
    const size_t MAX_CACHE_SIZE = 5000;

    int statusCode;
    QString message;

    std::string getFilePath() const;
    bool isEmailBlacklisted(const std::string &domain);
    void loadMailsFromFile();

    signals:
    void emailAvailable(bool isAvailable);
    void unableToCheckEmailAvailability();
};

class GetEmail : public QObject {
    Q_OBJECT

    public:
    explicit GetEmail(QObject *parent = nullptr);
    void setAccountCreateObject(AccountCreate *ac = nullptr);

    private:
    QTimer *timer = nullptr;
    EmailValidator *emailValidator = nullptr;
    AccountCreate *ac = nullptr;

    int retryAttempts = 0;
    QByteArray text;

    private slots:
    void onEmailChanged(const QString &pwd);
    void onEmailAvailable(bool isAvailable);    
    void onUnableToCheckEmailAvailability();
    void onTimeout();

    signals:
    void emailValidated(bool isValid);
};