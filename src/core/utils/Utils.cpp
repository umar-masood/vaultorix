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

    /* ------------------------------------------------  
                    ENV (BASIC) HANDLER
    -------------------------------------------------- */
            
    Utils::ENV::ENV(const QString &envName) {
       loadData(envName);
    }

    ENV& ENV::global(const QString &envName) {
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

