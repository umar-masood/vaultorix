#pragma once

#include <QObject>
#include <optional>
#include <QPointer>
#include <QNetworkAccessManager>
#include "../../../ui/vault/update/Update.h"

class LineProgress;

namespace Core::Services {
    /**
     * This class will:
     * - Sends get request to the server for checking is there any update available for this app?
     * - If the server respond with update available then it will show Update Now button available to the user otherwise it will shows no update available notice inside dailog.
     * - When the user clicks update now button, then new version of software will start downloading. Once it is done, we'll start installation by displaying a message to the user "We're installing an update, the app is going to close."
     * - In case if the app suddenly quit or crash due to unexpected behavior, we will remove the uncomplete download file from application directory file.
     */
    class AppUpdate : public QObject{
        Q_OBJECT

        public:
        explicit AppUpdate(QObject *parent = nullptr);
        void setAppUpdateWidget(Ui::Vault::AppUpdate *instance);

        private:
        QPointer<Ui::Vault::AppUpdate> updateWidget = nullptr;
        QNetworkAccessManager *manager = nullptr;
        QString updateDownloadUrl, filePath;
        QPointer<LineProgress> downloadProgressBar = nullptr;

        /**
         * This method will take current version and sends a get request to server to check is there any update available for this app?
         * 
         * If it responds with update metadata then it means update is available, so we have to display those update detials in update Ui.
         * 
         * @returns Update struct with update details added.
         */
        void isUpdateAvailable(const QString &currentVersion);
        
        /**
         * This method will start downloading the available update from remote.
         */
        void downloadUpdate();

        /**
         * When the update has been successfully downloaded, then this will install the update after closing app.
         */
        void installUpdate();

        /**
         * Gives us the location where the updates will be stored etc.
         */
        void updateFilePath();

        private slots:
        void onInternetConnectionError();
        void onUpdateAvailable(std::optional<Ui::Vault::Update> info);
        void onSomethingWentWrong();

        signals:
        void updateDownloaded();
        void noInternetConnection();
        void somethingWentWrong();
        void updateAvailable(std::optional<Ui::Vault::Update> info);
    };
};