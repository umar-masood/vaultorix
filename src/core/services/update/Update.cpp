#include "Update.h"

#include "../../config/Constants.h"
#include "../../config/APIConfig.h"
#include "../auth/TokenManager.h"
#include "../../utils/Utils.h"

#include "../../../ui/components/LineProgress.h"
#include "../../../ui/components/Button.h"

#include <QDateTime>
#include <QCoreApplication>
#include <QProcess>

using Core::AppUpdate;
using Core::TokenManager;

AppUpdate::AppUpdate(QObject *parent) : QObject(parent) {
    updateFilePath();

    manager = new QNetworkAccessManager(this);

    connect(this, &AppUpdate::updateAvailable, this, &AppUpdate::onUpdateAvailable);
    connect(this, &AppUpdate::updateDownloaded, this, &AppUpdate::installUpdate);
    connect(this, &AppUpdate::somethingWentWrong, this, &AppUpdate::onSomethingWentWrong);
    connect(this, &AppUpdate::noInternetConnection, this, &AppUpdate::onInternetConnectionError);
}

void AppUpdate::setAppUpdateWidget(Ui::Vault::AppUpdate *instance) {
    if (!instance)
        return;

    updateWidget = instance;
    downloadProgressBar = updateWidget->downloadProgressBar();

    isUpdateAvailable(APP_VERSION);

    connect(updateWidget->updateButton(), &Button::clicked, this, [this](){
        Utils::InternetConnectivity::instance().runIfOnline(
            [this](){
                downloadUpdate();
            },
            this,
            "",
            [this]() {
                emit noInternetConnection();
            }
        );
    });
}

void AppUpdate::isUpdateAvailable(const QString &currentVersion) {
    if (currentVersion.isEmpty())
        return;

    // Function for handling response from server
    auto responseCallable = [this](const QJsonObject &obj){
        if (obj.contains("status_code") && obj.contains("message")) {
            if (obj["status_code"].toInt() == 200) {
                INFO_HERE("No update is available.");
                emit updateAvailable(std::nullopt);
                return; 
            } else {
                DEBUG_HERE(QString::number(obj["status_code"].toInt()) + "   " + obj["message"].toString());
                emit somethingWentWrong();
            }
        } else {
            // Saving download link
            this->updateDownloadUrl = obj["download_url"].toString();
            
            double sizeBytes = obj["file_size"].toVariant().toLongLong();
            double sizeMB = sizeBytes / (1024.0 * 1024.0);
            
            Ui::Vault::Update update(
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
                responseCallable
            );
        else 
            emit noInternetConnection();
    }, Qt::SingleShotConnection);
}

void AppUpdate::downloadUpdate() {
    QUrl url(updateDownloadUrl);
    QNetworkRequest request(url);
    request.setTransferTimeout(REQUEST_TIMEOUT);

    QNetworkReply *reply = manager->get(request);
    reply->setParent(updateWidget);
   
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

    if (!updateWidget)
        return;

    updateWidget->downloadProgressBar()->setIndeterminate(true);
    updateWidget->downloadProgressBar()->start();

    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesReceived, qint64 bytesTotal){
        if (bytesTotal > 0) {
            int percent = static_cast<int>((bytesReceived * 100.0) / bytesTotal);

            if (!downloadProgressBar)
                return;

            if (downloadProgressBar->indeterminate())
                downloadProgressBar->setIndeterminate(false);

            downloadProgressBar->setValue(percent);
            downloadProgressBar->setText(QString::number(percent));
        }
    });

    connect(reply, &QNetworkReply::readyRead, this, [this, file, reply](){
        file->write(reply->readAll());
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply, file](){
        if (reply->error() == QNetworkReply::NoError) {
            file->flush();
            file->close();

            if (!updateWidget)
                return;
            
            downloadProgressBar->setValue(100);
            downloadProgressBar->setText(QString::number(100));
            downloadProgressBar->stop();
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
        QTimer::singleShot(15000, this, &AppUpdate::downloadUpdate);

        reply->abort();
        reply->disconnect();
        reply->deleteLater();
        file->deleteLater();
    });
}

void AppUpdate::onInternetConnectionError() {
    if (!updateWidget)
        return;

    updateWidget->setUpdateState(Ui::Vault::AppUpdate::UpdateState::NoInternet);
}

void AppUpdate::onSomethingWentWrong() {
    if (!updateWidget)
        return;

    updateWidget->setUpdateState(Ui::Vault::AppUpdate::UpdateState::SomethingWentWrong);
}

void AppUpdate::updateFilePath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Updates/Installer/" + QString(APP_VERSION);
    QDir().mkpath(path);
    this->filePath = (path + "/" + UPDATE_DOWNLOAD_FILE);
}

void AppUpdate::onUpdateAvailable(std::optional<Ui::Vault::Update> info) {
    if (!updateWidget)
        return;

    if (info) {
        updateWidget->setUpdateDetails(info.value());
        updateWidget->setUpdateState(Ui::Vault::AppUpdate::UpdateState::Available);
    } else 
        updateWidget->setUpdateState(Ui::Vault::AppUpdate::UpdateState::NotAvailable);
}

void AppUpdate::installUpdate() {
    if (!QFileInfo::exists(filePath))
        return;

    QProcess::startDetached(filePath);

    QTimer::singleShot(100, [](){
        QCoreApplication::quit();
    });
}