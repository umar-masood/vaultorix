#pragma once

#include <algorithm>
#include <filesystem>
#include <iostream>
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


namespace Utils {
    // General Helpers
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

            InternetConnectivity::instance().checkConnectivity();
        }

        signals:
        void connectivityChanged(bool isOnline);

        private:  
        InternetConnectivity();
        QNetworkAccessManager *_manager = nullptr;        
    };

    // BlacklistManager
    class BlacklistManager : public QObject {
        Q_OBJECT

        public:
        explicit BlacklistManager(const QString &blacklistName, QObject *parent = nullptr);

        void setFileName(const QString &name);
        bool downloadList(const QUrl &url);

        const QString filePath() const;

        signals:
        void networkError(QNetworkReply::NetworkError err);
        void listDownloaded();

        private:
        QString _filePath, _blacklistName;

        // Network Manager
        QNetworkAccessManager *_manager = nullptr;    

        bool isOlderList() const;
        void onFinished(QNetworkReply *reply);
    };

    /* ----------------------------------------------------
                    .env Parser
    ------------------------------------------------------- */
    class ENV {
        public:
        explicit ENV(const QString &envName);
        static ENV& instance(const QString &envName);

        void load(const QString &envName);
        QString get(const QString &key) const;

        private:
        QHash<QString, QString> _data;

        // Helper
        void loadData(const QString &envName);
    };
}