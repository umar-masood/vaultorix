#include "UpdateService.h"

#include "../../config/Constants.h"
#include "../../config/APIConfig.h"
#include "../auth/TokenManager.h"
#include "../../utils/Utils.h"

#include <QDateTime>
#include <QCoreApplication>
#include <QProcess>

using Core::UpdateService;
using Core::TokenManager;

UpdateService::UpdateService(QObject *parent) : QObject(parent) {
    updateFilePath();

    manager = new QNetworkAccessManager(this);

    connect(this, &UpdateService::updateDownloaded, this, &UpdateService::installUpdate);
}

UpdateService::UpdateData::UpdateData(const QString &currentVersion, 
                          const QString &newVersion,
                          const QString &size, 
                          const QDateTime &releasedDate, 
                          const QString &updateNotes) : 
                          _currentVersion(currentVersion),
                          _newVersion(newVersion),
                          _size(size),
                          _releasedDate(releasedDate),
                          _updateNotes(updateNotes)
{

}


void UpdateService::isUpdateAvailable(const QString &currentVersion) {
    if (currentVersion.isEmpty())
        return;

    // Function for handling response from server
    auto responseCallable = [this](const QJsonObject &obj){
        if (obj.contains("status_code") && obj.contains("message")) {
            if (obj["status_code"].toInt() == 200) {
                INFO_HERE("No update is available.");
                emit updateAvailable(std::nullopt);
                emit updateCurrentState(State::Unavailable);
                return; 
            } else {
                DEBUG_HERE(QString::number(obj["status_code"].toInt()) + "   " + obj["message"].toString());
                emit updateError(Error::SomethingWentWrong);
            }
        } else {
            // Saving download link
            this->updateDownloadUrl = obj["download_url"].toString();
            
            double sizeBytes = obj["file_size"].toVariant().toLongLong();
            double sizeMB = sizeBytes / (1024.0 * 1024.0);
            
            UpdateData update(
                APP_VERSION,
                obj["latest_version"].toString(),
                QString::number(
                    sizeMB, // Number to be represented
                    'f', // Fixed-point
                    2 // Decimal precision
                ) + " MB",
                QDateTime::fromString(obj["release_date"].toString(), "M-d-yyyy"),
                obj["release_notes"].toString()
            );

            emit updateAvailable(update);
            emit updateCurrentState(State::Available);
        }
    };

    using IC = Utils::InternetConnectivity; // Alias
    IC::instance().checkConnectivity();
    connect(&IC::instance(), &IC::connectivityChanged, this, [this, responseCallable](bool isOnline){
        if (isOnline)
            TokenManager::instance()->sendRequest(
                route(APIRoutes::CHK_UPDATE) + APP_VERSION,
                QByteArray(), // Empty Because of Get Request
                QNetworkAccessManager::GetOperation,
                responseCallable,
                    [this](QNetworkReply *reply) {
                        ERROR_HERE("Update check failed: " + reply->errorString());
                        emit updateError(Error::SomethingWentWrong);
                    }
            );
        else 
            emit updateError(Error::NoInternet);
    }, Qt::SingleShotConnection);
}

void UpdateService::downloadUpdate() {
    QUrl url(updateDownloadUrl);
    QNetworkRequest request(url);
    request.setTransferTimeout(REQUEST_TIMEOUT);

    emit updateDownloadStarted();

    QNetworkReply *reply = manager->get(request);
   
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) 
        QFile::remove(filePath);

    auto *file = new QFile(filePath);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        ERROR_HERE("Failed to open file for writing update.");
        reply->abort();
        reply->deleteLater();
        file->deleteLater();
        return;
    }

    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesReceived, qint64 bytesTotal){
        if (bytesTotal > 0) {
            int percent = static_cast<int>((bytesReceived * 100.0) / bytesTotal);
            emit updateDownloadProgress(percent);
        }
    });

    connect(reply, &QNetworkReply::readyRead, this, [this, file, reply](){
        file->write(reply->readAll());
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply, file](){
        if (reply->error() == QNetworkReply::NoError) {
            file->flush();
            file->close();

            emit updateDownloaded();
        } else {
            ERROR_HERE("Failed to complete downloading update : " + reply->errorString());
            QFile::remove(filePath);
        }

        reply->deleteLater();
        file->deleteLater();
    });

    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply, file](QNetworkReply::NetworkError error) {
        QFile::remove(filePath);

        // Restarting download
        QTimer::singleShot(15000, this, &UpdateService::downloadUpdate);

        reply->abort();
        reply->disconnect();

        reply->deleteLater();
        file->deleteLater();
    });
}

void UpdateService::updateFilePath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Updates/Installer/" + QString(APP_VERSION);
    QDir().mkpath(path);
    this->filePath = (path + "/" + UPDATE_DOWNLOAD_FILE);
}

void UpdateService::installUpdate() {
    if (!QFileInfo::exists(filePath))
        return;

    QProcess::startDetached(filePath);

    QTimer::singleShot(100, [](){
        QCoreApplication::quit();
    });
}