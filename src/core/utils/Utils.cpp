#include "Utils.h"
#include "../config/Constants.h"

namespace Utils {
    /*---------------------------------------------------------------
                        BLACKLIST MANAGER 
    -----------------------------------------------------------------*/
    BlacklistManager::BlacklistManager(QObject *parent, const QString &blacklistName) : QObject(parent) {
        manager = new QNetworkAccessManager(this);

        connect(this, &BlacklistManager::networkError, [this, blacklistName](QNetworkReply::NetworkError err) {
            qDebug() << "Downloading " << blacklistName << " blacklist failed: " << err << "\n";
        });
    }

    void BlacklistManager::setFileName(const std::string &filename) {
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Config";
        QDir().mkpath(path);
        filePath = path.toStdString() + "/" + filename;
    }

    bool BlacklistManager::isOlderList() {
        if (std::filesystem::exists(filePath)) {
            QFileInfo info(QString::fromStdString(filePath));
            QDateTime modified = info.lastModified();
            qint64 secsAgo = modified.secsTo(QDateTime::currentDateTime()); // Difference
            return secsAgo > OLDER_LIST_TIME;
        }
        return true;
    }

    bool BlacklistManager::downloadList(const QUrl &url) {
        if (!isOlderList())
            return false;

        QNetworkRequest request(url);
        QNetworkReply *reply = manager->get(request);

        connect(reply, &QNetworkReply::finished, this, [this, reply]() { onFinished(reply); });
        connect(reply, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError err) { emit networkError (err); });

        return true;
    }

    void BlacklistManager::onFinished(QNetworkReply *reply) {
        if (!reply)
            return;

        if (reply->error() != QNetworkReply::NoError) {
            QUrl url = reply->url();

            // Reattempt after 10 seconds to download again if failed
            QTimer::singleShot(REDOWNLOAD_INTERVAL, this, [this, url]() { downloadList(url); }); 

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
    };

    /*---------------------------------------------------------------
                        GENERAL HELPERS 
    -----------------------------------------------------------------*/
    void lower(std::string &str) {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    }

    void cleanupMemory(std::string &str) {
        if (str.empty())
            return;

        volatile char *p = const_cast<char *>(str.data());
        std::fill(p, p + str.size(), 0);

        str.clear();
        str.shrink_to_fit();
    }

    void cleanupMemory(QByteArray &bytes) {
        if (bytes.isEmpty())
            return;

        bytes.detach();
        volatile char *p = bytes.data();
        std::fill(p, p + bytes.size(), 0);

        bytes.clear();
        bytes.squeeze();
    }
    
    /*---------------------------------------------------------------
                             SECRETS MANAGER 
    -----------------------------------------------------------------*/
    // SecretsManager::SecretsManager(QObject *parent) : QObject(parent) {}

    // SecretsManager* SecretsManager::instance(QObject *parent) {
    //     static auto *sm = new SecretsManager(parent);
    //     return sm;
    // }

    // void SecretsManager::addSecret(const QString &key, const QString &secret) {
    //     auto *writer = new Writer(APP_NAME, this);
    //     writer->setKey(key);
    //     writer->setTextData(secret);
        
    //     connect(writer, &Writer::finished, this, [this, writer, key](){
    //         if (writer->error())
    //             emit secretAdded(key, false);
    //         else
    //             emit secretAdded(key, true);
            
    //         writer->deleteLater();
    //     }, Qt::SingleShotConnection);

    //     writer->start();
    // }

    // void SecretsManager::removeSecret(const QString &key) {
    //     auto *remover = new Remover(APP_NAME, this);
    //     remover->setKey(key);

    //     connect(remover, &Remover::finished, this, [this, remover, key](){
    //         if (remover->error())
    //             emit secretRemoved(key, false);
    //         else
    //             emit secretRemoved(key, true);

    //         remover->deleteLater();
    //     }, Qt::SingleShotConnection);

    //     remover->start();
    // }

    // void SecretsManager::retrieveSecret(const QString &key) {
    //     auto *reader = new Reader(APP_NAME, this);
    //     reader->setKey(key);

    //     connect(reader, &Reader::finished, this, [this, reader, key](){
    //         if (reader->error())
    //             emit secretRetrieved(key, std::unexpected(-1));
    //         else
    //             emit secretRetrieved(key, reader->textData());

    //         reader->deleteLater();
    //     }, Qt::SingleShotConnection);

    //     reader->start();
    // }

    /*---------------------------------------------------------------
                            INTERNET CONNECTIVITY
        -----------------------------------------------------------------*/
    InternetConnectivity& InternetConnectivity::instance() {
        static InternetConnectivity ic;
        return ic;
    }

    InternetConnectivity::InternetConnectivity() {
        manager = new QNetworkAccessManager(this);
    }

    void InternetConnectivity::checkConnectivity() {
        QNetworkRequest request(QUrl("https://www.google.com/generate_204"));
        request.setTransferTimeout(5000);

        QNetworkReply *reply = manager->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            bool online = reply->error() == QNetworkReply::NoError;
            emit connectivityChanged(online);
            reply->deleteLater();
        });
    }
}

