#include "NameValidator.h"
#include <QRegularExpression>

NameValidator::NameValidator(QObject *parent) : QObject(parent) { }

void NameValidator::checkNameValidity(const QString &name) {
    if (name.isEmpty()) {
        emit nameValidated(false);
        return;
    }

    if (name.length() < 3 || name.length() > 30) {
        emit nameValidated(false);
        return;
    }

    static const QRegularExpression regex("^[A-Z][A-Za-z ']+$");
    if (!regex.match(name).hasMatch()) {
        emit nameValidated(false);
        return;
    }

    emit nameValidated(true);
}
