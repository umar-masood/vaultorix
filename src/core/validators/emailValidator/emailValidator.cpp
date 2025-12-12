#include "emailValidator.h"

EmailValidator::EmailValidator() {
    vu = new ValidatorUtils(nullptr, "Email");
    vu->setFileName("badDomains.config");
    if (vu->downloadList(QUrl("https://raw.githubusercontent.com/umar-masood/Weak-Credentials/refs/heads/main/badDomains.config"))) 
       qDebug() << "Email blacklist download started.\n";
    else
        loadMailsFromFile();

    QObject::connect(vu, &ValidatorUtils::listDownloaded, [this]() {
        qDebug() << "Email blacklist download completed.\n";
        loadMailsFromFile();
    });
}

bool EmailValidator::isValidEmail(QByteArray &email) {
    if (email.isEmpty() || !email.contains('@'))
        return false;

    if (email.count('@') != 1)
        return false;

    if (email.contains(' ') || email.contains('\t'))
        return false;

    // Split email into local and domain parts
    int atIndex = email.indexOf('@');
    QByteArray local = email.left(atIndex);
    QByteArray domain = email.mid(atIndex + 1);

    if (local.isEmpty() || domain.isEmpty())
        return false;

    if (local.startsWith('.') || local.endsWith('.') || local.contains(".."))
        return false;

    if (domain.startsWith('.') || domain.endsWith('.') || domain.contains(".."))
        return false;

    if (!domain.contains('.'))
        return false;

    // Check all characters are ASCII
    for (auto c : email) 
        if (static_cast<unsigned char>(c) > 127)
            return false;

    // Regex patterns for local and domain
    static const QRegularExpression localRe("^[A-Za-z0-9._%+-]+$");
    static const QRegularExpression domainRe("^(?:[A-Za-z0-9-]+\\.)+[A-Za-z]{2,24}$");

    if (!localRe.match(QString::fromUtf8(local)).hasMatch())
        return false;

    if (!domainRe.match(QString::fromUtf8(domain)).hasMatch())
        return false;

    // Check individual domain labels
    QList<QByteArray> labels = domain.split('.');
    for (const QByteArray &label : labels) {
        if (label.startsWith('-') || label.endsWith('-') || label.isEmpty())
            return false;
    }

    // Securely wipe local and domain parts after use
    ValidatorUtils::cleanupMemory(local);
    ValidatorUtils::cleanupMemory(domain);

    return true;
}

bool EmailValidator::checkDisposableEmail(QByteArray &email) {
    if (!isValidEmail(email)) return true; // True invalid email (considered disposable)

    if (tempMails.empty()) {
        ValidatorUtils::cleanupMemory(email);
        std::cerr << "Disposable domain list is not loaded.\n";
        return true; // If disposable password list is not loaded, we cannot check for weak passwords
    }

    int atIndex = email.indexOf('@');
    QByteArray domainBA = email.mid(atIndex + 1);
    ValidatorUtils::cleanupMemory(email); 

    std::string domain(domainBA.constData(), domainBA.size());
    ValidatorUtils::lower(domain);

    // Securely wipe temporary QByteArray
    ValidatorUtils::cleanupMemory(domainBA);

    bool result = isDisposableEmail(domain);

    // Wipe temporary std::string
    ValidatorUtils::cleanupMemory(domain);

    return result;
}

bool EmailValidator::isDisposableEmail(const std::string &domain) {
    auto it = cacheMap.find(domain);
    if (it != cacheMap.end()){
       qDebug() << "Domain is already checked...";
        order.splice(order.end(), order, it->second); 
        return true;
    }

    bool result = tempMails.find(domain) != tempMails.end();
    if (result) {
        order.push_back(domain);
        cacheMap[domain] = std::prev(order.end()); 
        if (cacheMap.size() > MAX_CACHE_SIZE) {
            std::string oldestPwd = order.front();
            cacheMap.erase(oldestPwd);
            order.pop_front();
        }
    }

    return result;
}

void EmailValidator::loadMailsFromFile() {
    std::ifstream file(vu->filePath);
    if (!file.is_open()) {
        std::cerr << "Could not open domains list file.\n";
        return;
    }

    tempMails.reserve(71500);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            line.pop_back();
        ValidatorUtils::lower(line);
        if (!line.empty())
            tempMails.insert(line);
    }
    file.close();
   qDebug() << "Loaded " << tempMails.size() << " domains.\n";
}

GetEmail::GetEmail(QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &GetEmail::onTimeout);
}

void GetEmail::setAccountCreateObject(AccountCreate *ac) {
    if (!ac) return;
    this->ac = ac;
    connect(ac->emailField(), &CustomTextField::textChanged, this, &GetEmail::onEmailChanged);
}

void GetEmail::onEmailChanged(const QString &text) {
    Q_UNUSED(text)
    timer->stop();
    timer->start(2000);
}

void GetEmail::onTimeout() {
    if (!ac) return;
    
    QByteArray text = ac->emailField()->text().toUtf8();
    if (text.isEmpty()) {
        ac->emailField()->setUnchecked();
        ac->emailField()->setTooltip("");
        return;
    }

    bool ok = emailValidator.checkDisposableEmail(text);

    // Emit signal
    emit emailValidated(!ok);
    
    if (!ok) {
        ac->emailField()->setChecked();
        ac->emailField()->setTooltip("Valid email-address");
    } else {
        ac->emailField()->setUnchecked();
        ac->emailField()->setTooltip("Invalid email-address");
    }

    ValidatorUtils::cleanupMemory(text);
}