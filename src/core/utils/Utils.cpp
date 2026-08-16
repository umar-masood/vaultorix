#include "Utils.h"
#include "../config/Constants.h"
#include <openssl/crypto.h>

namespace Utils {
    /*---------------------------------------------------------------
                        BLACKLIST MANAGER 
    -----------------------------------------------------------------*/
    BlacklistManager::BlacklistManager(const QString &blacklistName, QObject *parent = nullptr) : QObject(parent), _blacklistName(blacklistName) {
        _manager = new QNetworkAccessManager(this);
    }

    void BlacklistManager::setFileName(const QString &name) {
        const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/config";
        QDir().mkpath(dir);
        _filePath = dir + "/" + name;
    }

    bool BlacklistManager::isOlderList() const {
        if (QFile::exists(_filePath)) {
            const QFileInfo info(_filePath);
            const QDateTime modified = info.lastModified();
            qint64 secsAgo = modified.secsTo(QDateTime::currentDateTime()); // Difference
            return secsAgo > OLDER_LIST_TIME;
        }

        return true;
    }

    bool BlacklistManager::downloadList(const QUrl &url) {
        if (!isOlderList())
            return false;

        QNetworkRequest request(url);
        QNetworkReply *reply = _manager->get(request);

        connect(reply, &QNetworkReply::finished, this, [this, reply]() { 
            onFinished(reply); 
        });
        
        connect(reply, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError err) { 
            qDebug() << "Downloading " << _blacklistName << " blacklist failed: " << err << "\n"; 
        });

        return true;
    }

    const QString BlacklistManager::filePath() const {
        return _filePath;
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

        QFile file(_filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            ERROR_HERE("Failed to write to file: " + _filePath);
            return;
        }

        if (file.write(data) != data.size()) {
            ERROR_HERE("Failed to completely write blacklist file.");
            return;
        }

        file.close();

        emit listDownloaded();  
    };

    /*---------------------------------------------------------------
                        GENERAL HELPERS 
    -----------------------------------------------------------------*/

    void cleanupMemory(QByteArray &bytes) {
        if (bytes.isEmpty())
            return;

        bytes.detach();
        
        OPENSSL_cleanse(bytes.data(), bytes.size());

        bytes.clear();
        bytes.squeeze();
    }
    
    /*---------------------------------------------------------------
                            INTERNET CONNECTIVITY
        -----------------------------------------------------------------*/
    InternetConnectivity& InternetConnectivity::instance() {
        static InternetConnectivity ic;
        return ic;
    }

    InternetConnectivity::InternetConnectivity() {
        _manager = new QNetworkAccessManager(this);
    }

    void InternetConnectivity::checkConnectivity() {
        QNetworkRequest request(QUrl("https://www.google.com/generate_204"));
        request.setTransferTimeout(5000);

        QNetworkReply *reply = _manager->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            emit connectivityChanged(reply->error() == QNetworkReply::NoError);
            reply->deleteLater();
        });
    }

    /* ------------------------------------------------  
                    ENV (BASIC) HANDLER
    -------------------------------------------------- */
            
    Utils::ENV::ENV(const QString &envName) {
       loadData(envName);
    }

    ENV& ENV::instance(const QString &envName) {
        static ENV env(envName);
        return env;
    }

    void ENV::load(const QString &envName) {
        loadData(envName);
    }

    QString ENV::get(const QString & key) const {   
        if (_data.empty())
            return "";
        
        return _data[key];
    }

    void ENV::loadData(const QString &envName) {
        if (envName.isEmpty()) {
            WARN_HERE("Your provided an empty .env name");
            return;
        }

        if (!QFile::exists(envName)) {
            ERROR_HERE("The .env file does not exist.");
            return;
        }

        QFile file(envName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            ERROR_HERE("Failed to read from .env file.");
            return;
        }

        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QStringList line = stream.readLine().split("=");
            if (line.size() < 2) {
                ERROR_HERE("Invalid .env file.");
                return;
            }

            _data[line[0].trimmed()] = line[1].trimmed();
        }

        file.close();
    }

}

