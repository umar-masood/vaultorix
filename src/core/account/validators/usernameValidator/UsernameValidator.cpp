#include "UsernameValidator.h"

UsernameValidator::UsernameValidator(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);

    vu = new ValidatorUtils(this, "Username");
    vu->setFileName("badUsernames.config");
    if (vu->downloadList(QUrl("https://raw.githubusercontent.com/umar-masood/Weak-Credentials/refs/heads/main/badUsernames.config")))
        qDebug() << "Username blacklist download started.\n";
    else
        loadUsernamesFromFile();
    
    connect(vu, &ValidatorUtils::listDownloaded, this, [this]() {
        qDebug() << "Username blacklist download completed.\n";
        loadUsernamesFromFile();
    });
}

bool UsernameValidator::isValidUsername(QByteArray &username) {
    //  Length check
    if (username.length() < 3 || username.length() > 20) {
        return false;
    }

    // First character must be a letter
    if (!std::isalpha(static_cast<unsigned char>(username.at(0)))) {
        return false;
    }

    // Start/end character restrictions
    if (username.startsWith("-") || username.startsWith(".") || username.startsWith("_") ||
        username.endsWith("-") || username.endsWith(".") || username.endsWith("_")) {
        return false;
    }

    // Non-ASCII check
    for (auto c : username) {
        if (static_cast<unsigned char>(c) > 127) {
            return false;
        }
    }

    // Allowed characters regex
    static const QRegularExpression pattern("^[A-Za-z0-9._-]+$");
    if (!pattern.match(QString::fromUtf8(username)).hasMatch()) {
        return false;
    }

    // Convert to lowercase safely
    std::string usernameStd = username.toStdString();                
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

void UsernameValidator::isUsernameAvailable(QByteArray &username) {
    // QUrl url("http://www.umarcreations.site");
    QUrl url("http://127.0.0.1:8000");
    url.setPath("/check-username/" + QString::fromUtf8(username));    

    qDebug() << "Checking username availability for: " << QString::fromUtf8(username) << "\n";
    QNetworkRequest request(url);
    request.setTransferTimeout(15000);

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply, &username](){
        if (!reply)
            return;

        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            qDebug() << "Error :  " << reply->errorString() << "\n";

            emit unableToCheckUsernameAvailability(); // Emit signal when there is an error while handling the request
            return;
        }

        QByteArray data = reply->readAll();
        reply->deleteLater();

        if (data.isEmpty()) {
            qDebug() << "Data is empty." << "\n"; 
            return;
        }

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "Json parsing error." << "\n";
            return;
        }

        if (!jsonDoc.isObject()) {
            qDebug() << "Invalid json data" << "\n";
            return;
        }

        QJsonObject obj = jsonDoc.object();
        message = obj["message"].toString();
        statusCode = obj["status_code"].toInt();
        
        qDebug() << "Username check response: " << message << " (Status Code: " << statusCode << ")\n";

        ValidatorUtils::cleanupMemory(username);
        emit usernameAvailable(statusCode == 200);
    });
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
    usernameValidator = new UsernameValidator(this);

    timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, this, &GetUsername::onTimeout);
    connect(usernameValidator, &UsernameValidator::usernameAvailable, this, &GetUsername::onUsernameAvailable);
    connect(usernameValidator, &UsernameValidator::unableToCheckUsernameAvailability, this, &GetUsername::onUnableToCheckUsernameAvailability);
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

void GetUsername::onUsernameAvailable(bool isAvailable) {
    if (isAvailable) {
        ac->usernameField()->setChecked();
        ac->usernameField()->setTooltip("Username is available.");
    } else {
        ac->usernameField()->setUnchecked();
        ac->usernameField()->setTooltip("Username is already taken.\nPlease use another.");
    }
    emit usernameValidated(isAvailable);
}

void GetUsername::onUnableToCheckUsernameAvailability() {
    if (retryAttempts < 3) {
        retryAttempts++;
        usernameValidator->isUsernameAvailable(text);
    } else {
        ac->usernameField()->setUnchecked();
        ac->usernameField()->setTooltip("Failed to check username availability.");
        retryAttempts = 0;

        ValidatorUtils::cleanupMemory(text);
        return;
    }
}
    
void GetUsername::onTimeout() {
    if (!ac) return;

    text = ac->usernameField()->text().toUtf8();

    if (text.isEmpty()) {
        ac->usernameField()->setUnchecked();
        ac->usernameField()->setTooltip("");
        return;
    }

    // Validate username
    bool ok = usernameValidator->isValidUsername(text);
    
    if (ok) {
        qDebug() << "Username is valid: " << text << "\n";
        ac->usernameField()->setTooltip("");
        
        usernameValidator->isUsernameAvailable(text);
    } else {
        ac->usernameField()->setUnchecked();
        ac->usernameField()->setTooltip(
        "Invalid username.\n\n"
        "Please ensure that:\n"
        "• 3-20 characters, starting with a letter\n"
        "• Letters, numbers, . _ - only\n"
        "• Cannot start or end with . _ -\n"
        "• No excessive character repetition\n"
        "• Must not be reserved"
        );

        qDebug() << "Username is not Valid: " << text << "\n";

        emit usernameValidated(false); 
        ValidatorUtils::cleanupMemory(text);
    }
    
}

