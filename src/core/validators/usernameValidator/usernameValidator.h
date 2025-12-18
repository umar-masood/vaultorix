#pragma once

#include "../../../ui/accountCreate/AccountCreate.h"
#include "../validatorUtils/validatorUtils.h"

#include <unordered_set>
#include <fstream>

#include <QRegularExpression>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

class UsernameValidator : public QObject {
    Q_OBJECT
    
public:
    explicit UsernameValidator(QObject *parent = nullptr);
    bool isValidUsername(QByteArray &username);
    void isUsernameAvailable(QByteArray &username);

private:
    ValidatorUtils *vu = nullptr;
    QNetworkAccessManager *manager = nullptr;

    QString message;
    int statusCode;

    std::unordered_set<std::string> tempUsernames;
    bool isUsernameBlacklisted(const std::string &username) const;
    void loadUsernamesFromFile();

    signals:
    void usernameAvailable(bool isavailable);
    void unableToCheckUsernameAvailability();
};

class GetUsername : public QObject {
    Q_OBJECT

public:
    explicit GetUsername(QObject *parent = nullptr);
    void setAccountCreateObject(AccountCreate *ac = nullptr);

private:
    QTimer *timer = nullptr;
    UsernameValidator *usernameValidator = nullptr;
    AccountCreate *ac = nullptr;

    int retryAttempts = 0;
    QByteArray text;

signals:
    void usernameValidated(bool isValid);
    
private slots:
    void onUsernameChanged(const QString &text);
    void onUsernameAvailable(bool isAvailable);
    void onUnableToCheckUsernameAvailability();
    void onTimeout();
};
