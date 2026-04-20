#pragma once

#include <QRegularExpression>
#include "../../utils/Utils.h"

namespace Ui::Auth { class Signup; };

/* -------------- Name Validator --------------- */
/**
 * This class will check the name passed by the GetName class, whether it is valid?
 */
class NameValidator {
    public:
    bool isValidName(const QByteArray &bytes);
};

/* ---------------- Get Name ---------------------*/
/**
 * This class will take name from full name textfield in Account Sign up widget in Ui and passed it to the actual Name validator class for validation
 */
class GetName : public QObject {
    Q_OBJECT

    public:
    explicit GetName(QObject *parent = nullptr);
    void setAccountSignupWidget(Ui::Auth::Signup *instance);

    signals:
    void nameValidated(bool isValid);

    private:
    // Current Account Create
    Ui::Auth::Signup *signupWidget = nullptr;

    // Timer
    QTimer *timer = nullptr;

    // Name Validator
    NameValidator nameValidator;

    // Slots
    void onNameChanged(const QString &text);
    void onTimeout();
};

