#pragma once

#include <QRegularExpression>

#include "../../../ui/auth/signup/Signup.h"
#include "../../utils/ValidatorUtils.h"

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
    void setAccountSignup(Signup *instance);

    signals:
    void nameValidated(bool isValid);

    private:
    // Current Account Create
    Signup *ac = nullptr;

    // Timer
    QTimer *timer = nullptr;

    // Name Validator
    NameValidator nameValidator;

    // Slots
    void onNameChanged(const QString &text);
    void onTimeout();
};

