#pragma once

#include <QObject>
#include <optional>
#include <QPointer>
#include <QNetworkAccessManager>
#include <QString>

class LineProgress;

namespace Core {
    /**
     * This class will:
     * - Sends get request to the server for checking is there any update available for this app?
     * - If the server respond with update available then it will show Update Now button available to the user otherwise it will shows no update available notice inside dailog.
     * - When the user clicks update now button, then new version of software will start downloading. Once it is done, we'll start installation by displaying a message to the user "We're installing an update, the app is going to close."
     * - In case if the app suddenly quit or crash due to unexpected behavior, we will remove the uncomplete download file from application directory file.
     */
    class UpdateService : public QObject{
        Q_OBJECT

        public:
        enum class Error {
            NoInternet,
            SomethingWentWrong
        };

        enum class State {
            Available,
            Unavailable
        };

        struct UpdateData {
            QString _currentVersion = "0.0.0";
            QString _newVersion = "0.0.0";
            QString _size = "0 MB";
            QDateTime _releasedDate;
            QString _updateNotes;

            UpdateData(const QString &currentVersion,
                       const QString &newVersion,
                       const QString &size,
                       const QDateTime &releasedDate,
                       const QString &updateNotes
            );
        };

        explicit UpdateService(QObject *parent = nullptr);
        void isUpdateAvailable(const QString &currentVersion);
        void downloadUpdate();

        private:
        QNetworkAccessManager *manager = nullptr;
        QString updateDownloadUrl, filePath;

        void installUpdate();
        void updateFilePath();

        signals:
        void updateDownloaded();
        void updateCurrentState(const State &state);
        void updateError(const Error &error);
        void updateAvailable(std::optional<UpdateData> data);
        void updateDownloadProgress(int percent);
        void updateDownloadStarted();
    };
};