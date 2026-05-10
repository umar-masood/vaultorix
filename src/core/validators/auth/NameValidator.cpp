#include "NameValidator.h"

NameValidator::NameValidator(QObject *parent) : QObject(parent) { }

void NameValidator::checkNameValidity(const QString &name) {
    bool valid = true;

    if (name.isEmpty()) {
        emit nameValidated(false);
        return;
    }

    if (name.length() < 3 || name.length() > 50)
        valid = false;

    if (name.contains("  "))
        valid = false;

    if (name.startsWith(' ') || name.endsWith(' '))
        valid = false;

    static const QRegularExpression re("^[A-Za-z ']+$");
    if (!re.match(name).hasMatch())
        valid = false;

    QChar ch = name.at(0);
    if (ch < 'A' || ch > 'Z')
        valid = false;

    emit nameValidated(valid);
}
