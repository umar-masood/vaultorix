#pragma once

#include <QRegularExpression>
#include <QObject>

class NameValidator  : public QObject {
    Q_OBJECT

    public:
    explicit NameValidator(QObject *parent = nullptr);
    void checkNameValidity(const QString &name);

    signals:
    void nameValidated(bool isValid);
};
