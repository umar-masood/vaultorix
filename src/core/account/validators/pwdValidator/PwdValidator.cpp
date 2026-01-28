#include "PwdValidator.h"

/* -------------------- Password Validator --------------------- */
PwdValidator::PwdValidator(QObject *parent) : QObject(parent) {
    vu = new ValidatorUtils(this, "Password");
    vu->setFileName("badPwds.config");
    if (vu->downloadList(QUrl("https://raw.githubusercontent.com/umar-masood/Weak-Credentials/refs/heads/main/badPwds.config"))) 
        qDebug() << "Password blacklist downloaded started.\n";
    else
        loadPwdsFromFile();

    connect(vu, &ValidatorUtils::listDownloaded, this, [this]() {
        qDebug() << "Password blacklist download completed.\n";
        loadPwdsFromFile();
    });
}

bool PwdValidator::isValidPwd(QByteArray &pwdBytes, PwdRulesWidget *pwdRules) {    
    bool hasLength = pwdBytes.size() >= 8;
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    for (char ch : pwdBytes) {
        unsigned char c = static_cast<unsigned char>(ch);
        if (c >= 'A' && c <= 'Z')
            hasUpper = true;
        else if (c >= 'a' && c <= 'z')
            hasLower = true;
        else if (c >= '0' && c <= '9')
            hasDigit = true;
        else
            hasSpecial = true;
    }

    hasLength ? pwdRules->atLeastEight()->setChecked() : pwdRules->atLeastEight()->setUnchecked();
    hasUpper ? pwdRules->atLeastOneUpperCaseChar()->setChecked() : pwdRules->atLeastOneUpperCaseChar()->setUnchecked();
    hasLower ? pwdRules->atLeastOneLowerCaseChar()->setChecked() : pwdRules->atLeastOneLowerCaseChar()->setUnchecked();
    hasDigit ? pwdRules->atLeastOneDigit()->setChecked() : pwdRules->atLeastOneDigit()->setUnchecked();
    hasSpecial ? pwdRules->atLeastOneSpecialChar()->setChecked() : pwdRules->atLeastOneSpecialChar()->setUnchecked();

    for (int i = 0; i < pwdBytes.size(); i++) 
        pwdBytes[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(pwdBytes[i])));

    std::string pwdStd = pwdBytes.toStdString();    
    ValidatorUtils::cleanupMemory(pwdBytes);

    if (weakPwds.empty()) {
        ValidatorUtils::cleanupMemory(pwdStd);
        pwdRules->strongPassword()->setUnchecked();
        qDebug() << "Weak password list is not loaded.\n";
        return false; // If weak password list is not loaded, we cannot check for weak passwords
    }

    bool notWeak = !isPwdBlacklisted(pwdStd);
    ValidatorUtils::cleanupMemory(pwdStd);

    bool isStrongPwd = hasLength && hasUpper && hasLower && hasDigit && hasSpecial && notWeak;
    isStrongPwd ? pwdRules->strongPassword()->setChecked() : pwdRules->strongPassword()->setUnchecked();

    return isStrongPwd;
}

bool PwdValidator::isPwdBlacklisted(const std::string &password) {
    auto it = cacheMap.find(password); // We make a search in the cache map to see if the password has been checked before using an iterator.
    if (it != cacheMap.end()) { // If the password is found in the cache map (means the iterator does not reached at the end of the map)
        qDebug() << "Password is already checked...";
        order.splice(order.end(), order, it->second); /* Splice method is used to move an element from one position to aother. It takes three arguments where the first 
        argument is the destination (where) you want to move value, second argument is the current data structure and the last one is the value which you want to move.
        Basically here we are moving the iterator(it->second) which points to the password in the list to the end of the list (most recently used position).
        In the start of list, all previously used passwords are stored in the order of their usage (least recently used at front and most recently used at end).
        So, when user enters a password which is already checked, we move that password to the end of the list to mark it as most recently used.
        */
        return true; // True because password is weak
    }

    bool result = weakPwds.find(password) != weakPwds.end(); // In case if the entered password is not found in the cache map, we check it in the weak password set.
    // When a weak password is found in the set
    if (result) {
        order.push_back(password); // We will add the new weak password in the order list 
        cacheMap[password] = std::prev(order.end()); /* std::prev returns the element before end(), here we are add iterator in map that points to the last inserted password at the end in the list */

        if (cacheMap.size() > MAX_CACHE_SIZE) { // If the size of the cache map exceeds the maximum limit defined
            std::string oldestPwd = order.front(); // We get the oldest password from the front of the list
            cacheMap.erase(oldestPwd); // We remove that oldest password from the cache map
            order.pop_front(); // We also remove that oldest password from the order list to maintain the size of the cache
        }
    }

    return result; 
}

void PwdValidator::loadPwdsFromFile() {
    std::ifstream file(vu->filePath);
    if (!file.is_open()) {
        qDebug() << "Could not open weak pwd list file.\n";
        return;
    }

    weakPwds.reserve(131554);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            line.pop_back();
        ValidatorUtils::lower(line);
        if (!line.empty())
            weakPwds.insert(line);
    }
    
    file.close();
    qDebug() << "Loaded " << weakPwds.size() << " weak passwords.\n";
}

/* -----------------------  Get Password ----------------------- */
GetPassword::GetPassword(QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    timer->setSingleShot(true);

    pwdValidate = new PwdValidator(this);

    connect(timer, &QTimer::timeout, this, [this](){
        if (!ac) return;
        
        QByteArray bytes = ac->passwordField()->text().toUtf8();
        bool ok = pwdValidate->isValidPwd(bytes, ac->passwordValidatorWidget());
        
        // Emit signal
        emit pwdValidated(ok);

        ValidatorUtils::cleanupMemory(bytes);
    });
}

void GetPassword::setAccountCreateObject(AccountCreate *ac) {
    if (!ac) return;
    this->ac = ac;
    connect(ac->passwordField(), &CustomTextField::textChanged, this, &GetPassword::onPwdChanged);
}

void GetPassword::onPwdChanged(const QString &text) {
    Q_UNUSED(text);
    timer->stop();
    timer->start(2000);
}