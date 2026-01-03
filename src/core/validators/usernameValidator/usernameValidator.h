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

/* ----------------- Username Validator -------------------- */
class UsernameValidator : public QObject {
    Q_OBJECT
    
    public:
    explicit UsernameValidator(QObject *parent = nullptr);
    bool isValidUsername(QByteArray &username);
    void isUsernameAvailable(QByteArray &username);

    private:
    // Validator Utils
    ValidatorUtils *vu = nullptr;

    // Network Manager
    QNetworkAccessManager *manager = nullptr;

    // API Response
    QString message;
    int statusCode;

    // Stores temporary usernames fetched using an API
    std::unordered_set<std::string> tempUsernames;

    // Helper Methods
    bool isUsernameBlacklisted(const std::string &username) const;
    void loadUsernamesFromFile();

    signals:
    void usernameAvailable(bool isavailable);
    void unableToCheckUsernameAvailability();
};

/* ---------------  Get Username ------------------ */
class GetUsername : public QObject {
    Q_OBJECT

    public:
    explicit GetUsername(QObject *parent = nullptr);
    void setAccountCreateObject(AccountCreate *ac = nullptr);

    private:
    // Timer to delay username checking on every key stroke being pressed
    QTimer *timer = nullptr;

    // Username Validator
    UsernameValidator *usernameValidator = nullptr;

    // Current Account Create
    AccountCreate *ac = nullptr;

    // Retry Attempts Tracker
    int retryAttempts = 0;

    // Stores username from TextField
    QByteArray text;

    // Slots
    void onUsernameChanged(const QString &text);
    void onUsernameAvailable(bool isAvailable);
    void onUnableToCheckUsernameAvailability();
    void onTimeout();

    signals:
    void usernameValidated(bool isValid);
};
