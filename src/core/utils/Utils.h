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

// #include "../../../dependencies/qtkeychain/keychain.h"
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

        template<typename RequestCallable, typename NoInternetCallable = std::nullptr_t> 

         /**
         * This method is used to run a network request after checking whether there is an active internet connection?
         * 
         * If there is no, then it will show error dialog of no internet connectivity, we must pass current window key to correctly parented the dailog with its main window ("Auth for AuthWindow  AND   Vault for VaultWindow")
         * 
         * In case if we want to call our own function in case of no internet connection established then we can passed it as noInternetCallable
         * 
         * 
         * Logic:
         * We want the second template argument should be optional, as we know templates uses default type not default value and then they pass that type to function etc., 
         * 
         * Here we have passed default std::nullptr_t which only stores one value that is nullptr
         * So when user does pass noInternetCallable then template deduces its default type which will be std::nullptr_t and then inside function parameter, it will assign a default value which will be nullptr
         */
        void runIfOnline(RequestCallable requestCallable, 
                        QObject *parent = nullptr, 
                        const QString &dialogCurrentWindowKey = "Auth",
                        NoInternetCallable noInternetCallable = nullptr
                    ) 
        {
            static_assert(std::is_invocable_v<RequestCallable> , "runIfOnline requires a request callable.");

            if constexpr(!std::is_same_v<NoInternetCallable, std::nullptr_t>)
                static_assert(std::is_invocable_v<NoInternetCallable> , "runIfOnline requires a no internet callable.");

            InternetConnectivity::instance().checkConnectivity();

            connect(&InternetConnectivity::instance(), &InternetConnectivity::connectivityChanged, parent, 
            [requestCallable, noInternetCallable, dialogCurrentWindowKey](bool isOnline) {

                if (isOnline)
                    requestCallable();
                else {
                    if (ErrorDialogManager::instance() && !dialogCurrentWindowKey.isEmpty())
                        ErrorDialogManager::instance()->show("NoInternet", dialogCurrentWindowKey);

                    if constexpr (!std::is_same_v<NoInternetCallable, std::nullptr_t>) 
                        noInternetCallable();
                }
                
            }, Qt::SingleShotConnection);
        }

        signals:
        void connectivityChanged(bool isOnline);

        private:  
        InternetConnectivity();
        QNetworkAccessManager *manager = nullptr;        
    };

    // // Secrets Manager
    // class SecretsManager : public QObject {
    //     Q_OBJECT

    //     using Writer = QKeychain::WritePasswordJob;
    //     using Reader = QKeychain::ReadPasswordJob;
    //     using Remover = QKeychain::DeletePasswordJob;
        
    //     SecretsManager(QObject *parent = nullptr);

    //     public:
    //     static SecretsManager* instance(QObject *parent);

    //     void addSecret(const QString &key, const QString &secret);
    //     void removeSecret(const QString &key);
    //     void retrieveSecret(const QString &key);

    //     signals:
    //     void secretAdded(const QString &key, bool isAdded);
    //     void secretRetrieved(const QString &key, std::expected<QString, int>);
    //     void secretRemoved(const QString &key, bool isRemoved);
    // };

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