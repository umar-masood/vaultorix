#pragma once

#include "../../../ui/accountCreate/AccountCreate.h"
#include "../validatorUtils/validatorUtils.h"

#include <string>
#include <unordered_set>
#include <iostream>
#include <fstream>

#include <QTimer>
#include <QString>
#include <QRegularExpression>

class UsernameValidator {

public:
    UsernameValidator();
    bool isValidUsername(QByteArray &username);

private:
    ValidatorUtils *vu = nullptr;

    std::unordered_set<std::string> tempUsernames;
    bool isUsernameBlacklisted(const std::string &username) const;
    void loadUsernamesFromFile();
};

class GetUsername : public QObject {
    Q_OBJECT

public:
    explicit GetUsername(QObject *parent = nullptr);
    void setAccountCreateObject(AccountCreate *ac = nullptr);

private:
    QTimer *timer = nullptr;
    UsernameValidator usernameValidator;
    AccountCreate *ac = nullptr;

signals:
    void usernameValidated(bool isValid);
    
private slots:
    void onUsernameChanged(const QString &text);
    void onTimeout();
};
