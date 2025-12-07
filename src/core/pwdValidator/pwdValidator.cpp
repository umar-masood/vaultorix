#include "pwdValidator.h"

PwdValidator::PwdValidator() {
    vu = new ValidatorUtils(nullptr, "Password");
    vu->setFileName("badPwds.config");
    if (vu->downloadList(QUrl("https://raw.githubusercontent.com/umar-masood/Weak-Credentials/refs/heads/main/badPwds.config"))) 
        qDebug() << "Password blacklist downloaded started.\n";
    else
        loadPwdsFromFile();

    QObject::connect(vu, &ValidatorUtils::listDownloaded, [this]() {
        qDebug() << "Password blacklist download completed.\n";
        loadPwdsFromFile();
    });
}

bool PwdValidator::checkStrongPwd(QByteArray &pwdBytes, PwdRulesWidget *pwdRules) {    
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
    hasUpper ? pwdRules->oneUpperCase()->setChecked() : pwdRules->oneUpperCase()->setUnchecked();
    hasLower ? pwdRules->oneLowerCase()->setChecked() : pwdRules->oneLowerCase()->setUnchecked();
    hasDigit ? pwdRules->oneDigit()->setChecked() : pwdRules->oneDigit()->setUnchecked();
    hasSpecial ? pwdRules->oneSpecialChar()->setChecked() : pwdRules->oneSpecialChar()->setUnchecked();

    for (int i = 0; i < pwdBytes.size(); i++) 
        pwdBytes[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(pwdBytes[i])));

    std::string pwdStd = pwdBytes.toStdString();    
    ValidatorUtils::cleanupMemory(pwdBytes);

    if (weakPwds.empty()) {
        ValidatorUtils::cleanupMemory(pwdStd);
        pwdRules->strongPwd()->setUnchecked();
        qDebug() << "Weak password list is not loaded.\n";
        return false; // If weak password list is not loaded, we cannot check for weak passwords
    }

    bool notWeak = !isWeakPwd(pwdStd);
    ValidatorUtils::cleanupMemory(pwdStd);

    bool isStrongPwd = hasLength && hasUpper && hasLower && hasDigit && hasSpecial && notWeak;
    isStrongPwd ? pwdRules->strongPwd()->setChecked() : pwdRules->strongPwd()->setUnchecked();

    return isStrongPwd;
}

bool PwdValidator::isWeakPwd(const std::string &password) {
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

GetPassword::GetPassword(QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    timer->setSingleShot(true);

    pwdValidate = new PwdValidator;

    connect(timer, &QTimer::timeout, this, [this](){
        if (!ac) return;
        QByteArray bytes = ac->pwdField()->text().toUtf8();
        bool ok = pwdValidate->checkStrongPwd(bytes, ac->pwdRulesWidget());
        ValidatorUtils::cleanupMemory(bytes);
    });
}

void GetPassword::setAccountCreateObject(AccountCreate *ac) {
    if (!ac) return;
    this->ac = ac;
    connect(ac->pwdField(), &CustomTextField::textChanged, this, &GetPassword::onPwdChanged);
}

void GetPassword::onPwdChanged(const QString &text) {
    Q_UNUSED(text);
    timer->stop();
    timer->start(2000);
}