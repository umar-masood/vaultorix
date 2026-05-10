#include "ReportBug.h"

#include "../../../ui/vault/report_bug/ReportBug.h"
#include "../../../ui/components/TextField.h"
#include "../../../ui/components/Button.h"
#include "../../../ui/components/TextEdit.h"
#include "../../config/Constants.h"
#include "../../config/APIConfig.h"
#include "../auth/TokenManager.h"
#include "../../utils/Utils.h"

#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFile>
#include <QFileInfo>

using Core::ReportBug;
using Core::TokenManager;

ReportBug::ReportBug(QObject *parent) : QObject(parent){
    connect(this, &ReportBug::somethingWentWrong, this, &ReportBug::onSomethingWentWrong);
}

void ReportBug::setReportBugWidget(Ui::Vault::ReportBug *reportBug) {
    if (!reportBug)
        return;

    reportBugWidget = reportBug;

    connect(reportBugWidget, &Ui::Vault::ReportBug::reportFinished, this, &ReportBug::sendBugReport);
}

void ReportBug::sendBugReport() {
    // Changing Button State to loader
    changeSubmitBtnState("", false);

    // Fetching data
    fetchData();

    auto multipartCallable = [this]() {
        // Preparing Multipart Request
        QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        // Subject Part
        QHttpPart subPart;
        subPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"subject\""));
        subPart.setBody(subject.toUtf8());
        multipart->append(subPart);

        // Description Part
        QHttpPart descpPart;
        descpPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"description\""));
        descpPart.setBody(descp.toUtf8());
        multipart->append(descpPart);

        // App Version
        QHttpPart appVersionPart;
        appVersionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"app_version\""));
        appVersionPart.setBody(APP_VERSION);
        multipart->append(appVersionPart);

        // Loading Screenshot
        if (!screenshotPath.isEmpty()) {
            QFile *file = new QFile(screenshotPath);
            file->setParent(multipart);

            if (!file->exists() || !file->open(QIODevice::ReadOnly)) {
                ERROR_HERE("Unable to load the screenshot file.";)
                delete file;
            }
        
            // Screenshot Info
            QFileInfo fileInfo(screenshotPath);
            QString fileName = fileInfo.fileName();
            QString fileType = fileInfo.suffix().toLower();
            QString mimeType;

            // Checking Screenshot types
            if (fileType == "png")
                mimeType = "image/png";
            else if (fileType == "jpg" || fileType == "jpeg")
                mimeType = "image/jpeg";

            // Screenshot Part
            QHttpPart screenshotPart;
            screenshotPart.setHeader(QNetworkRequest::ContentTypeHeader, mimeType.toUtf8());
            screenshotPart.setHeader(QNetworkRequest::ContentDispositionHeader, 
                                    QVariant(QString("form-data; name=\"screenshot\"; filename=\"%1\"").arg(fileName))
                                );
            screenshotPart.setBodyDevice(file);
            multipart->append(screenshotPart);
        }

        return multipart;
    };

    // Network Request Server Response Handler Callable
    auto responseCallable = [this](const QJsonObject &obj) {
        if (obj.contains("status_code") && obj.contains("message")) 
            if (obj["status_code"].toInt() == 200) {
                INFO_HERE("Your bug report has been submitted");
                changeSubmitBtnState("Submitted", false);
                reportBugWidget->close();
                return; 
            } else {
                // This will handle the server related error
                DEBUG_HERE(obj["status_code"].toString() + "   " + obj["message"].toString());
                changeSubmitBtnState("Submit", true);
                emit somethingWentWrong(); // Overall we are emitting this same signal for all types of error excluding internet connectivity.
            }
    };

    // Network Request Failure Callable
    auto networkRequestFailureCallable = [this](QNetworkReply *reply) {
        emit somethingWentWrong();
    };

    // Network Request
    Utils::InternetConnectivity::instance().runIfOnline(
        [this, responseCallable, multipartCallable, networkRequestFailureCallable]() {
            TokenManager::instance()->sendMultipartRequest(
                route(APIRoutes::REPORT_BUG),
                multipartCallable,
                QNetworkAccessManager::PostOperation,
                responseCallable,
                networkRequestFailureCallable
            );
        },
        this,
        "ReportBug", 
        [this](){
            changeSubmitBtnState("Submit", true);
        }
    );
}

void ReportBug::fetchData() {
    if (!reportBugWidget)
        return;

    subject = reportBugWidget->subject_field()->text();
    descp = reportBugWidget->descp_field()->document()->toPlainText();
    screenshotPath = reportBugWidget->filePath();
}

void ReportBug::changeSubmitBtnState(const QString &buttonText, bool isEnabled) {
    reportBugWidget->submit_btn()->setText(buttonText);
    reportBugWidget->submit_btn()->setEnabled(isEnabled);
}

void ReportBug::onSomethingWentWrong() {
    changeSubmitBtnState("Submit", true);
    ErrorDialogManager::instance()->show("SomethingWentWrong", "ReportBug");
}