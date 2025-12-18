#include "emailValidator.h"

EmailValidator::EmailValidator(QObject *parent) : QObject(parent) {
    vu = new ValidatorUtils(this, "Email");
    manager = new QNetworkAccessManager(this);
    
    vu->setFileName("badDomains.config");
    if (vu->downloadList(QUrl("https://raw.githubusercontent.com/umar-masood/Weak-Credentials/refs/heads/main/badDomains.config"))) 
       qDebug() << "Email blacklist download started.\n";
    else
        loadMailsFromFile();

    connect(vu, &ValidatorUtils::listDownloaded, this, [this]() {
        qDebug() << "Email blacklist download completed.\n";
        loadMailsFromFile();
    });
}

bool EmailValidator::isValidEmail(QByteArray &email) {
    // Basic email validation checks
    if (email.isEmpty() || !email.contains('@')) 
        return false;

    if (email.count('@') != 1)
        return false;

    if (email.contains(' ') || email.contains('\t'))
        return false;
    
    if (email.size() > 254)
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

    // Check all characters are in valid range - ASCII
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

    // Check if email is disposable
    if (tempMails.empty()) {
        qDebug() << "Disposable domain list is not loaded.\n";
        return false; // If disposable email list is not loaded, we cannot check
    }

    // Extract domain part
    std::string domainStr(domain.constData(), domain.size());
    ValidatorUtils::lower(domainStr);

    // Check if the domain is disposable
    bool isDisposable = isEmailBlacklisted(domainStr);

    // Securely wipe temporary std::string and QByteArray
    ValidatorUtils::cleanupMemory(domainStr);
    ValidatorUtils::cleanupMemory(domain);
    ValidatorUtils::cleanupMemory(local);

    // Return true if email is valid and not disposable
    return !isDisposable;
}

bool EmailValidator::isEmailBlacklisted(const std::string &domain) {
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

void EmailValidator::isEmailAvailable(QByteArray &email) {
    QUrl url("http://127.0.0.1:8000");
    url.setPath("/check-email/" + QString::fromUtf8(email));    

    qDebug() << "Checking email availability for: " << QString::fromUtf8(email) << "\n";
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply, &email](){
        if (!reply)
            return;

        if (reply->error() != QNetworkReply::NoError) {
            // Error detected
            reply->deleteLater();
            qDebug() << "Error :  " << reply->errorString() << "\n";

            emit unableToCheckEmailAvailability(); // Emit signal when there is an error while handling the request
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
        
        qDebug() << "Email check response: " << message << " (Status Code: " << statusCode << ")\n";

        ValidatorUtils::cleanupMemory(email); // Cleaning the memory after successfult request handled
        emit emailAvailable(statusCode == 200);
    });
}

void EmailValidator::loadMailsFromFile() {
    std::ifstream file(vu->filePath);
    if (!file.is_open()) {
        qDebug() << "Could not open domains list file.\n";
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
    emailValidator = new EmailValidator(this);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &GetEmail::onTimeout);
}

void GetEmail::setAccountCreateObject(AccountCreate *ac) {
    if (!ac) return;
    this->ac = ac;
    connect(ac->emailField(), &CustomTextField::textChanged, this, &GetEmail::onEmailChanged);
    connect(emailValidator, &EmailValidator::emailAvailable, this, &GetEmail::onEmailAvailable);
    connect(emailValidator, &EmailValidator::unableToCheckEmailAvailability, this, &GetEmail::onUnableToCheckEmailAvailability);
}

void GetEmail::onEmailChanged(const QString &text) {
    Q_UNUSED(text)
    timer->stop();
    timer->start(2000);
}

void GetEmail::onEmailAvailable(bool isAvailable) {
    if (isAvailable) {
        ac->emailField()->setChecked();
        ac->emailField()->setTooltip("Email-address is available");
    } else {
        ac->emailField()->setUnchecked();
        ac->emailField()->setTooltip("Email-address already exists");
    }
    
    emit emailValidated(isAvailable);
}

void GetEmail::onUnableToCheckEmailAvailability() {
    /* Every time, when this slot is called, it will increment the counter by 1. 
    In each call of this function it will check the retryAttempts are less than 3*/
    if (retryAttempts < 3) {
        retryAttempts++;
        emailValidator->isEmailAvailable(text); // It will call recursively isEmailAvailable() to check weather that entered email is available
    } else {
        /* if the retryAttempts limit reached, setUnchecked(), 
        set update tooltip text and update retryAttempts to 0 , so that 
        next time, when user entered text is changed then it will again check email availability in case of failure in handling
        network request by calling isEmailAvailable() recursively.*/
        ac->emailField()->setUnchecked();
        ac->emailField()->setTooltip("Failed to check email availability.");
        retryAttempts = 0; // Resetting conter

        ValidatorUtils::cleanupMemory(text); // We're clearning the memory when max retry attempts limit reached
        return;
    }
}

void GetEmail::onTimeout() {
    if (!ac) return;
    
    text = ac->emailField()->text().toUtf8(); // Getting data from TextField
    
    // In case if there's no text in the TextField
    if (text.isEmpty()) {
        ac->emailField()->setUnchecked(); 
        ac->emailField()->setTooltip(""); // No tooltip will show when it is set to empty string
        return;
    }

    bool ok = emailValidator->isValidEmail(text); // Returns true if the user entered email is correct.

    if (ok) { // if it is True (it means email format is correct, now we have to check its availability.)
        qDebug() << "Email is Valid: " << text << "\n";

        ac->emailField()->setTooltip(""); /* We have set empty string to tooltip (Reason: If we don`t do this, then the following behaviour might occurs:
            Suppose, first time, user enters invalid email, then tooltip will show "Invalid Email-Address" -> its fine
            BUT 
            Next time, when user enters correct email address, then it still shows "Invalid email-address" until the availability check is finished.
            On the basis of which, final message will show in tooltip.
        */
        emailValidator->isEmailAvailable(text); // Check weather this email exists in DB
    } else {
        // if it is False (Invalid Email), thus we don`t need to check its availability anymore.
        ac->emailField()->setUnchecked();
        ac->emailField()->setTooltip("Invalid email-address");
        qDebug() << "Email is not Valid: " << text << "\n";

        emit emailValidated(false);    
        ValidatorUtils::cleanupMemory(text); // Cleaning data
    }
}