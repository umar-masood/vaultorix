#include "UsernameValidator.h"
#include "../../config/APIConfig.h"
#include "../../config/Constants.h"
#include "../../../ui/utils/Utils.h"

#include <QRegularExpression>
#include <QTextStream>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

UsernameValidator::UsernameValidator(QObject *parent) : QObject(parent) {
    _manager = new QNetworkAccessManager(this);

    _blacklistManager = new Utils::BlacklistManager("Username", this);
    _blacklistManager->setFileName(USERNAME_BLACKLIST_FILE);

    if (_blacklistManager->downloadList(QUrl(QString::fromUtf8(APIRoutes::USERNAME_BLACKLIST))))
        INFO_HERE("Username blacklist is downloading.");
    else
        loadUsernamesFromFile();

    connect(_blacklistManager, &Utils::BlacklistManager::listDownloaded, this, [this]() {
        INFO_HERE("Username blacklist has been downloaded.");
        loadUsernamesFromFile();
    });
}

bool UsernameValidator::isValidUsername(const QString &username) const {
    if (username.length() < 3 || username.length() > 20 || !username.at(0).isLetter())
        return false;

    if (username.endsWith('-') || username.endsWith('.') || username.endsWith('_'))
        return false;

    static const QRegularExpression regex("^[A-Za-z0-9._-]+$");
    if (!regex.match(username).hasMatch())
        return false;

    const QString usernameLower(username.toLower());

    // Checking consecutives
    int count = 1;
    for (auto i = 1; i < usernameLower.size(); i++) {
        if (usernameLower[i] == usernameLower[i - 1]) {
            count++;
            if (count >= 3)
                return false;
        } else {
            count = 1;
        }
    }

    if (_blacklistedUsernames.empty()) {
        ERROR_HERE("Username blacklist is empty.");
        return false;
    }

    return _blacklistedUsernames.find(usernameLower) == _blacklistedUsernames.end();
}

void UsernameValidator::checkUsernameValidity(const QString &username) {
    if (!isValidUsername(username)) {
        emit usernameInvalid();
        return;
    }

    QNetworkRequest request(QUrl(route(APIRoutes::CHK_USERNAME) + username));
    request.setTransferTimeout(REQUEST_TIMEOUT);

    QNetworkReply *reply = _manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, username]() {
        if (!reply)
            return;

        if (reply->error() != QNetworkReply::NoError) {
            ERROR_HERE("Network request failed: " + reply->errorString());
            reply->deleteLater();
            emit failedToValidateUsername();
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

        const QJsonObject obj = doc.object();
        const int statusCode = obj["status_code"].toInt();

        DEBUG_HERE(QString::number(statusCode) + "   " + obj["message"].toString());

        emit usernameAvailable(statusCode == 200);
    });
}

void UsernameValidator::loadUsernamesFromFile() {
    QFile file(_blacklistManager->filePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ERROR_HERE("Unable to open username blacklist.");
        return;
    }

    _blacklistedUsernames.clear();
    _blacklistedUsernames.reserve(430);

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed().toLower();
        if (!line.isEmpty())
            _blacklistedUsernames.insert(line);
    }

    INFO_HERE("Loaded " + QString::number(_blacklistedUsernames.size()) + " usernames.");
}
