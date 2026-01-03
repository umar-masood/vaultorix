#include "ValidatorUtils.h"

ValidatorUtils::ValidatorUtils(QObject *parent, const QString &blacklistName) : QObject(parent) {
    manager = new QNetworkAccessManager(this);

    connect(this, &ValidatorUtils::networkError, [this, blacklistName](QNetworkReply::NetworkError err) {
        qDebug() << "Downloading " << blacklistName << " blacklist failed: " << err << "\n";
    });
}

void ValidatorUtils::setFileName(const std::string &filename) {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/config";
    QDir().mkpath(path);
    filePath = path.toStdString() + "/" + filename;
}

bool ValidatorUtils::isOlderList() {
    if (std::filesystem::exists(filePath)) {
        QFileInfo info(QString::fromStdString(filePath));
        QDateTime modified = info.lastModified();
        qint64 secsAgo = modified.secsTo(QDateTime::currentDateTime());
        return secsAgo > 48 * 60 * 60;
    }
    return true;
}

bool ValidatorUtils::downloadList(const QUrl &url) {
    if (!isOlderList())
        return false;

    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onFinished(reply); });
    connect(reply, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError err) { emit networkError(err); });

    return true;
}

void ValidatorUtils::onFinished(QNetworkReply *reply) {
    if (!reply)
        return;

    if (reply->error() != QNetworkReply::NoError) {
        QUrl url = reply->url();

        // Reattempt after 10 seconds to download again if failed
        QTimer::singleShot(10000, this, [this, url]() { downloadList(url); }); 

        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    if (data.isEmpty())
        return;

    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        std::cerr << "Could not write to file: " << filePath << "\n";
        return;
    }

    file.write(data);
    file.close();

    emit listDownloaded();  
}

void ValidatorUtils::lower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
}

void ValidatorUtils::cleanupMemory(std::string &str) {
    if (str.empty())
        return;

    volatile char *p = const_cast<char *>(str.data());
    std::fill(p, p + str.size(), 0);

    str.clear();
    str.shrink_to_fit();
}

void ValidatorUtils::cleanupMemory(QByteArray &bytes) {
    if (bytes.isEmpty())
        return;

    bytes.detach();
    volatile char *p = bytes.data();
    std::fill(p, p + bytes.size(), 0);

    bytes.clear();
    bytes.squeeze();
}
