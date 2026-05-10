#pragma once

#include "../../utils/Utils.h"

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
    void checkUsernameValidityAndAvailability(const QString &username);

    private:
    // Validator Utils
    Utils::BlacklistManager *blacklistManager = nullptr;

    // Network Manager
    QNetworkAccessManager *manager = nullptr;

    // Stores temporary usernames
    std::unordered_set<std::string> tempUsernames;

    // Helper
    void loadUsernamesFromFile();
    bool isValidUsername(const QString &username);

    signals:
    void usernameInvalid();
    void usernameAvailable(bool isAvailable);
    void failedToCheckUsername();
};