#pragma once

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <expected>
#include <functional>

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QStandardPaths>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QObject>

#include "../../../dependencies/qtkeychain/keychain.h"
#include "../../ui/dialogs/error_dialog/ErrorDialog.h"

namespace Utils {
    // General Helpers
    void lower(std::string &str);
    void cleanupMemory(std::string &str);
    void cleanupMemory(QByteArray &bytes);

    // Internet Connectivity Class
    class InternetConnectivity : public QObject {
        Q_OBJECT

        public:
        static InternetConnectivity& instance();
        void checkConnectivity();

        template<typename Function>
        void runIfOnline(Function func, QObject *parent = nullptr, ErrorDialogManager *errorManager = nullptr) {
            static_assert(std::is_invocable_v<Function> , "runIfOnline requires a callable.");
            InternetConnectivity::instance().checkConnectivity();
            connect(&InternetConnectivity::instance(), &InternetConnectivity::connectivityChanged, parent, 
            [func, errorManager](bool isOnline) {
                if (isOnline)
                    func();
                else {
                    if (errorManager)
                        errorManager->show("NoInternet", "Auth");
                }
            }, Qt::SingleShotConnection);
        }

        signals:
        void connectivityChanged(bool isOnline);

        private:  
        InternetConnectivity();

        QNetworkAccessManager *manager = nullptr;        

    };

    // Secrets Manager
    class SecretsManager : public QObject {
        Q_OBJECT

        using Writer = QKeychain::WritePasswordJob;
        using Reader = QKeychain::ReadPasswordJob;
        using Remover = QKeychain::DeletePasswordJob;
        
        SecretsManager(QObject *parent = nullptr);

        public:
        static SecretsManager* instance(QObject *parent);

        void addSecret(const QString &key, const QString &secret);
        void removeSecret(const QString &key);
        void retrieveSecret(const QString &key);

        signals:
        void secretAdded(const QString &key, bool isAdded);
        void secretRetrieved(const QString &key, std::expected<QString, int>);
        void secretRemoved(const QString &key, bool isRemoved);
    };

    // BlacklistManager
    class BlacklistManager : public QObject {
        Q_OBJECT

        public:
        explicit BlacklistManager(QObject *parent = nullptr, const QString &blacklistName = "");

        std::string filePath;

        void setFileName(const std::string &filename);
        bool downloadList(const QUrl &url);

        signals:
        void networkError(QNetworkReply::NetworkError err);
        void listDownloaded();

        private:
        // Network Manager
        QNetworkAccessManager *manager = nullptr;    

        void onFinished(QNetworkReply *reply);
        bool isOlderList();
    };

}