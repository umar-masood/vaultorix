#pragma once

#include <string>
#include <QUrl>
#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QObject>


class ValidatorUtils : public QObject
{
    Q_OBJECT

    public:
    explicit ValidatorUtils(QObject *parent = nullptr, const QString &alias = "");
    
    std::string filePath;

    void setFileName(const std::string &filename);
    bool isOlderList();
    bool downloadList(const QUrl &url);

    static void lower(std::string &str);
    static void cleanupMemory(std::string &str);
    static void cleanupMemory(QByteArray &bytes);

    signals:
    void networkError(QNetworkReply::NetworkError err);
    void listDownloaded();

    private slots:
    void onFinished(QNetworkReply *reply);

    private:
    QString alias;
    QNetworkAccessManager *manager = nullptr;
};
