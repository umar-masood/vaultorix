#pragma once

#include "../../ui/accountCreate/AccountCreate.h"
#include "../../ui/accountOTP/AccountOTP.h"
#include "../../ui/accountSignIn/AccountSignIn.h"
#include "../../ui/accountWindow/AccountWindow.h"

class AccountAuthCoordinator : QObject {
    Q_OBJECT

    public:
    AccountAuthCoordinator(QObject *parent = nullptr);

};