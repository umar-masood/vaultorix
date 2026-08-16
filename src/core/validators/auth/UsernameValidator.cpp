#include "UsernameValidator.h"
#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../../ui/utils/Utils.h"

UsernameValidator::UsernameValidator(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);

    blacklistManager = new Utils::BlacklistManager(this, "Username");
    blacklistManager->setFileName(USERNAME_BLACKLIST_FILE);

    if (blacklistManager->downloadList(QUrl(QString::fromUtf8(APIRoutes::USERNAME_BLACKLIST))))
        INFO_HERE("Username blacklist is being downloaded.");
    else
        loadUsernamesFromFile();

    connect(blacklistManager, &Utils::BlacklistManager::listDownloaded, this, [this]() {
        INFO_HERE("Username blacklist has been downloaded.");
        loadUsernamesFromFile();
    });
}

bool UsernameValidator::isValidUsername(const QString &username) {
    if (username.length() < 3 || username.length() > 20)
        return false;

    if (!username.at(0).isLetter())
        return false;

    if (username.startsWith("-") || username.startsWith(".") || username.startsWith("_") ||
        username.endsWith("-") || username.endsWith(".") || username.endsWith("_"))
        return false;

    for (QChar c : username)
        if (c.unicode() > 127)
            return false;

    static const QRegularExpression pattern("^[A-Za-z0-9._-]+$");
    if (!pattern.match(username).hasMatch())
        return false;

    std::string usernameStd = username.toStdString();
    Utils::lower(usernameStd);

    if (usernameStd.size() > 1) {
        int count = 1;
        for (size_t i = 0; i + 1 < usernameStd.size(); i++) {
            if (usernameStd[i] == usernameStd[i + 1]) {
                if (++count >= 3)
                    return false;
            } else {
                count = 1;
            }
        }
    }

    if (tempUsernames.empty()) {
        ERROR_HERE("Username blacklist is not loaded.");
        return false;
    }

    return tempUsernames.find(usernameStd) == tempUsernames.end();
}

void UsernameValidator::checkUsernameValidityAndAvailability(const QString &username) {
    if (!isValidUsername(username)) {
        emit usernameInvalid();
        return;
    }
    QNetworkRequest request(QUrl(route(APIRoutes::CHK_USERNAME) + username));
    request.setTransferTimeout(REQUEST_TIMEOUT);

    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, username]() {
        if (!reply)
            return;

        if (reply->error() != QNetworkReply::NoError) {
            ERROR_HERE("Network request failed: " + reply->errorString());
            reply->deleteLater();
            emit failedToCheckUsername();
            return;
        }

        QByteArray data = reply->readAll();
        reply->deleteLater();

        if (data.isEmpty())
            return;

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject())
            return;

        QJsonObject obj = doc.object();
        int statusCode = obj["status_code"].toInt();

        DEBUG_HERE(QString::number(statusCode) + "   " + obj["message"].toString());

        emit usernameAvailable(statusCode == 200);
    });
}

void UsernameValidator::loadUsernamesFromFile() {
    std::ifstream file(blacklistManager->filePath);
    if (!file.is_open()) {
        ERROR_HERE("Unable to open username blacklist.");
        return;
    }

    tempUsernames.clear();
    tempUsernames.reserve(430);

    std::string line;
    while (std::getline(file, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            line.pop_back();

        Utils::lower(line);

        if (!line.empty())
            tempUsernames.insert(line);
    }

    INFO_HERE("Loaded " + QString::number(tempUsernames.size()) + " usernames.");
}
