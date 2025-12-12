#include "UsernameValidator.h"

UsernameValidator::UsernameValidator() {
    vu = new ValidatorUtils(nullptr, "Username");
    vu->setFileName("badUsernames.config");
    if (vu->downloadList(QUrl("https://raw.githubusercontent.com/umar-masood/Weak-Credentials/refs/heads/main/badUsernames.config")))
        qDebug() << "Username blacklist download started.\n";
    else
        loadUsernamesFromFile();
    
    QObject::connect(vu, &ValidatorUtils::listDownloaded, [this]() {
        qDebug() << "Username blacklist download completed.\n";
        loadUsernamesFromFile();
    });
}

bool UsernameValidator::isValidUsername(QByteArray &username) {
    //  Length check
    if (username.length() < 3 || username.length() > 20) {
        ValidatorUtils::cleanupMemory(username);
        return false;
    }

    // First character must be a letter
    if (!std::isalpha(static_cast<unsigned char>(username.at(0)))) {
        ValidatorUtils::cleanupMemory(username);
        return false;
    }

    // Start/end character restrictions
    if (username.startsWith("-") || username.startsWith(".") || username.startsWith("_") ||
        username.endsWith("-") || username.endsWith(".") || username.endsWith("_")) {
        ValidatorUtils::cleanupMemory(username);
        return false;
    }

    // Non-ASCII check
    for (auto c : username) {
        if (static_cast<unsigned char>(c) > 127) {
            ValidatorUtils::cleanupMemory(username);
            return false;
        }
    }

    // Allowed characters regex
    static const QRegularExpression pattern("^[A-Za-z0-9._-]+$");
    if (!pattern.match(QString::fromUtf8(username)).hasMatch()) {
        ValidatorUtils::cleanupMemory(username);
        return false;
    }

    // Convert to lowercase safely
    std::string usernameStd = username.toStdString();                
    ValidatorUtils::cleanupMemory(username);

    ValidatorUtils::lower(usernameStd);

    // Check for repeated characters (>=3)
    int count = 1;
    for (size_t i = 0; i < usernameStd.size() - 1; i++) {
        if (usernameStd[i] == usernameStd[i + 1]) {
            count++;
            if (count >= 3) {
                ValidatorUtils::cleanupMemory(usernameStd);
                return false;
            }
        } else {
            count = 1;
        }
    }

    // In case if there was an issue loading the blacklist (not found or not downloaded)
    if (tempUsernames.empty()) {
        ValidatorUtils::cleanupMemory(usernameStd);
        qDebug() << "Username blacklist is not loaded.\n";
        return false;
    }

    // Blacklist check
    bool blacklisted = isUsernameBlacklisted(usernameStd);

    ValidatorUtils::cleanupMemory(usernameStd);

    return !blacklisted;
}


bool UsernameValidator::isUsernameBlacklisted(const std::string &username) const {
    return tempUsernames.find(username) != tempUsernames.end(); // Username is blacklisted (True if found)
}

void UsernameValidator::loadUsernamesFromFile() {
    std::ifstream file(vu->filePath);
    if (!file.is_open()) {
        qDebug() << "Could not open usernames list file.\n";
        return;
    }

    tempUsernames.reserve(430);
    std::string line;
    while (std::getline(file, line)) {
        // Remove line endings
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            line.pop_back();

        ValidatorUtils::lower(line);

        if (!line.empty())
            tempUsernames.insert(line);
    }
    file.close();

    qDebug() << "Loaded " << tempUsernames.size() << " usernames.\n";
}

/* ==================== Get Username ==================== */
GetUsername::GetUsername(QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &GetUsername::onTimeout);
}

void GetUsername::setAccountCreateObject(AccountCreate *ac) {
    if (!ac) return;
    this->ac = ac;
    connect(ac->usernameField(), &CustomTextField::textChanged, this, &GetUsername::onUsernameChanged);
}

void GetUsername::onUsernameChanged(const QString &text) {
    Q_UNUSED(text)
    timer->stop();
    timer->start(2000); 
}

void GetUsername::onTimeout() {
    if (!ac) return;

    QByteArray text = ac->usernameField()->text().toUtf8();

    if (text.isEmpty()) {
        ac->usernameField()->setUnchecked();
        ac->usernameField()->setTooltip("");
        return;
    }

    // Validate username
    bool ok = usernameValidator.isValidUsername(text);

    // Emit signal
    emit usernameValidated(ok);
    
    // Clear sensitive memory
    ValidatorUtils::cleanupMemory(text);
    
    if (ok) {
        ac->usernameField()->setChecked();
        ac->usernameField()->setTooltip("Valid username");
    } else {
        ac->usernameField()->setUnchecked();
        ac->usernameField()->setTooltip("Invalid username");
    }
}
