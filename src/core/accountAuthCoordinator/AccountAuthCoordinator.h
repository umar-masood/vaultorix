#pragma once

#include "../../ui/accountCreate/AccountCreate.h"
#include "../../ui/accountOTP/AccountOTP.h"
#include "../../ui/accountSignIn/AccountSignIn.h"
#include "../../ui/accountWindow/AccountWindow.h"
#include <QSettings>

class AccountAuthCoordinator : QObject {
    Q_OBJECT

    public:
    AccountAuthCoordinator(QObject *parent = nullptr);

    private:
    QSettings settings;
    const QString key = "account/registered";

    void setAccountRegistered(bool isRegistered);
    bool isAccountRegistered() const;
};