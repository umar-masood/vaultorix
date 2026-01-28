#pragma once

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>

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

class ValidatorUtils : public QObject {
    Q_OBJECT

    public:
    explicit ValidatorUtils(QObject *parent = nullptr, const QString &blacklistName = "");
    
    std::string filePath;

    void setFileName(const std::string &filename);
    bool downloadList(const QUrl &url);
    static void lower(std::string &str);
    static void cleanupMemory(std::string &str);
    static void cleanupMemory(QByteArray &bytes);

    signals:
    void networkError(QNetworkReply::NetworkError err);
    void listDownloaded();

    private:
    // Network Manager
    QNetworkAccessManager *manager = nullptr;    
    
    // Slot
    void onFinished(QNetworkReply *reply);

    // Helper Methods
    bool isOlderList();

};
