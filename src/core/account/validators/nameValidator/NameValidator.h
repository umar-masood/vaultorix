#pragma once

#include <QRegularExpression>

#include "../../../../ui/account/accountCreate/AccountCreate.h"
#include "../validatorUtils/ValidatorUtils.h"

/* -------------- Name Validator --------------- */
class NameValidator {
    public:
    bool isValidName(const QByteArray &bytes);
};

/* ---------------- Get Name ---------------------*/
class GetName : public QObject {
    Q_OBJECT

    public:
    explicit GetName(QObject *parent = nullptr);
    void setAccountCreateObject(AccountCreate *ac);

    signals:
    void nameValidated(bool isValid);

    private:
    // Current Account Create
    AccountCreate *ac = nullptr;

    // Timer
    QTimer *timer = nullptr;

    // Name Validator
    NameValidator nameValidator;

    // Slots
    void onNameChanged(const QString &text);
    void onTimeout();
};

