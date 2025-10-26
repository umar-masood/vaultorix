#pragma once

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <string>

#include "../../ui/pwdRulesWidget/pwdRulesWidget.h"

class PwdValidator {
public:
    explicit PwdValidator();

    bool checkStrongPwd(std::string password);
    bool isWeakPwd(const std::string &password);

private:
    std::unordered_set<std::string> weakPwds;
    std::list<std::string> order;
    std::unordered_map<std::string, std::list<std::string>::iterator> cacheMap;
    const size_t MAX_CACHE_SIZE = 1000;
    PwdRulesWidget pwdRules;

    std::string getFilePath();
    static void lower(std::string &str);
    bool isOlderList();
    bool downloadList();
    void loadPwdsFromFile();
    void cleanupMemory(std::string &str);
    void cleanupMemory(QByteArray &bytes);
};
