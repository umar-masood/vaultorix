#pragma once

#include <QByteArray>
#include <QString>
#include <QRegularExpression>
#include <QObject>
#include <QTimer>

#include "../../ui/accountCreate/AccountCreate.h"
#include "../validatorUtils/validatorUtils.h"

class NameValidator {
    public:
    bool isValidName(const QByteArray &bytes);
};

class GetName : public QObject {
    Q_OBJECT
    public:
    explicit GetName(QObject *parent = nullptr);
    void setAccountCreateObject(AccountCreate *ac);

    private slots:
    void onNameChanged(const QString &text);
    void onTimeout();

    private:
    AccountCreate *ac = nullptr;
    QTimer *timer = nullptr;
    NameValidator nameValidator;
};

