#include "pwdValidator.h"

PwdChecker::PwdChecker() {
    if (downloadList()) 
        std::cout << "List downloaded successfully.\n";
    else 
        std::cerr << "Using local file or failed to download.\n";
    
    loadPwdsFromFile();
}

bool PwdChecker::checkStrongPwd(std::string password) {
    QByteArray pwdBytes = QByteArray::fromStdString(password);
    cleanupMemory(password);
    
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

    hasLength ? pwdRules.atLeastEight()->setChecked() : pwdRules.atLeastEight()->setUnchecked();
    hasUpper ? pwdRules.oneUpperCase()->setChecked() : pwdRules.oneUpperCase()->setUnchecked();
    hasLower ? pwdRules.oneLowerCase()->setChecked() : pwdRules.oneLowerCase()->setUnchecked();
    hasDigit ? pwdRules.oneDigit()->setChecked() : pwdRules.oneDigit()->setUnchecked();
    hasSpecial ? pwdRules.oneSpecialChar()->setChecked() : pwdRules.oneSpecialChar()->setUnchecked();

    for (int i = 0; i < pwdBytes.size(); i++) 
        pwdBytes[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(pwdBytes[i])));

    bool notWeak = !isWeakPwd(pwdBytes.toStdString());
    bool isStrongPwd = hasLength && hasUpper && hasLower && hasDigit && hasSpecial && notWeak;
    isStrongPwd ? pwdRules.strongPwd()->setChecked() : pwdRules.strongPwd()->setUnchecked();

    cleanupMemory(pwdBytes);

    return isStrongPwd;
}

bool PwdChecker::isWeakPwd(const std::string &password) {
    auto it = cacheMap.find(password);
    if (it != cacheMap.end()) {
        std::cout << "Password is already checked...";
        order.splice(order.end(), order, it->second); /* splice is used for moving an element from one place to another, while
            it->second point to the value of map in key-value pair , which has further an itertor pointing to the same password in the list*/
        return true;
    }

    bool result = weakPwds.find(password) != weakPwds.end();

    if (result) {
        order.push_back(password);
        cacheMap[password] = std::prev(order.end()); /*std::prev returns the element before end(), here we are add iterator in map that points to the last inserted password at the end in the list*/

        if (cacheMap.size() > MAX_CACHE_SIZE) {
            std::string oldestPwd = order.front();
            cacheMap.erase(oldestPwd);
            order.pop_front();
        }
    }

    return result;
}

std::string PwdChecker::getFilePath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/config";
    QDir().mkpath(path);
    return path.toStdString() + "/weakPwds.config";
}

void PwdChecker::lower(std::string &str) {
    for (auto &c : str) c = std::tolower(static_cast<unsigned char>(c));
}

bool PwdChecker::isOlderList() {
    if (std::filesystem::exists(getFilePath())) {
        QFileInfo info(QString::fromStdString(getFilePath()));
        QDateTime modified = info.lastModified();
        return modified.daysTo(QDateTime::currentDateTime()) > 1;
    }
    return true;
}

bool PwdChecker::downloadList() {
    if (!isOlderList()) return true;

    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("https://raw.githubusercontent.com/umar-masood/Weak-Passwords/refs/heads/main/weakPwds.config"));
    QNetworkReply *reply = manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec(); 

    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        std::cerr << reply->errorString().toStdString();
        return false;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    if (data.isEmpty()) return false;

    QFile file(QString::fromStdString(getFilePath()));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        std::cerr << "Could not write to file.\n";
        return false;
    }

    file.write(data);
    file.close();
    return true;
}

void PwdChecker::loadPwdsFromFile() {
    std::ifstream file(getFilePath());
    if (!file.is_open()) {
        std::cerr << "Could not open weak pwd list file.\n";
        return;
    }

    weakPwds.reserve(10500);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            line.pop_back();
        lower(line);
        if (!line.empty())
            weakPwds.insert(line);
    }
    file.close();
    std::cout << "Loaded " << weakPwds.size() << " weak passwords.\n";
}

void PwdChecker::cleanupMemory(std::string &str) {
    if (str.empty()) return;
    volatile char *p = str.data();
    for (size_t i = 0; i < str.size(); i++) {
        p[i] = 0;
    }
}

void PwdChecker::cleanupMemory(QByteArray &bytes) {
    if (bytes.isEmpty()) return;
    volatile char *p = bytes.data();
    for (size_t i = 0; i < bytes.size(); i++) {
        bytes[i] = 0;
    }
    bytes.clear();
}
