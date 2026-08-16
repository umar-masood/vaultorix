#pragma once

#include "../../utils/Utils.h"
#include <QSet>

class UsernameValidator : public QObject {
    Q_OBJECT
    
    public:
    explicit UsernameValidator(QObject *parent = nullptr);
    void checkUsernameValidity(const QString &username);

    private:
    // BlackList Manager
    Utils::BlacklistManager *_blacklistManager = nullptr;

    // Network Manager
    QNetworkAccessManager *_manager = nullptr;

    // Unordered Set to store Temp Usernames
    QSet<QString> _blacklistedUsernames;

    // Helpers
    void loadUsernamesFromFile();
    bool isValidUsername(const QString &username) const;

    signals:
    void usernameInvalid();
    void usernameAvailable(bool isAvailable);
    void failedToValidateUsername();
};