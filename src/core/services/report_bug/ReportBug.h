#pragma once
#include <QObject>
#include <QPointer>
#include <QNetworkReply>

namespace Ui::Vault { class ReportBug; };
namespace Core {
    class ReportBug : public QObject{
        Q_OBJECT

        public:
        explicit ReportBug(QObject *parent = nullptr);
        void setReportBugWidget(Ui::Vault::ReportBug *reportBug);

        private:
        QPointer<Ui::Vault::ReportBug> reportBugWidget = nullptr;
        QString subject, descp, screenshotPath;

        void sendBugReport();
        void fetchData();
        void changeSubmitBtnState(const QString &buttonText, bool isEnabled);

        signals:
        void somethingWentWrong();

        private slots:
        void onSomethingWentWrong();
    };
};