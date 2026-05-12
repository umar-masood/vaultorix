#pragma once
#include <QObject>
#include <QPointer>
#include <QNetworkReply>

namespace Core {
    class ReportBugService : public QObject{
        Q_OBJECT

        public:
        struct BugData {
            QString subject;
            QString description;
            QString screenshotPath;
        };

        explicit ReportBugService(QObject *parent = nullptr);
        void sendBugReport(const BugData &data);

        signals:
        void somethingWentWrong();
        void reportSubmitted();
        void noInternet();
    };
};