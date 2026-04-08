#include "NameValidator.h"

/* -----------  Name Validator -------------------  */
bool NameValidator::isValidName(const QByteArray &bytes) {
    if (bytes.isEmpty())
        return false;

    if (bytes.length() < 3 || bytes.length() > 50)
        return false;

    if (bytes.contains("  "))
        return false;

    if (bytes.startsWith(' ') || bytes.endsWith(' '))
        return false;

    static const QRegularExpression re("^[A-Za-z ']+$");
    if (!re.match(QString::fromUtf8(bytes)).hasMatch())
        return false;

    unsigned char ch = bytes[0];
    if (ch < 'A' || ch > 'Z')
        return false;

    return true;
}

/* ----------------- GetName ----------------------  */
GetName::GetName(QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &GetName::onTimeout);
}

void GetName::setAccountSignup(Signup *instance) {
    if (!instance) 
        return;
    
    ac = instance;
    
    connect(ac->nameField(), &CustomTextField::textChanged, this, &GetName::onNameChanged);
}

void GetName::onNameChanged(const QString &text) {
    Q_UNUSED(text)
    timer->stop();
    timer->start(2000);
}

void GetName::onTimeout() {
    if (!ac) return;

    QByteArray text = ac->nameField()->text().toUtf8();

    if (text.isEmpty()) {
        ac->nameField()->setInvalid();
        ac->nameField()->setTooltip("");
        return;
    }

    bool ok = nameValidator.isValidName(text);

    // Emit signal
    emit nameValidated(ok);

    Utils::cleanupMemory(text);

    if (ok) {
        ac->nameField()->setValid();
        ac->nameField()->setTooltip("Valid full name");
    } else {
        ac->nameField()->setInvalid();
        ac->nameField()->setTooltip("Invalid full name");
    }
}
