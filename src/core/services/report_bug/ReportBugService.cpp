#include "ReportBugService.h"

#include "../../config/Constants.h"
#include "../../config/APIConfig.h"
#include "../auth/TokenManager.h"
#include "../../utils/Utils.h"

#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFile>
#include <QFileInfo>

using Core::ReportBugService;
using Core::TokenManager;

ReportBugService::ReportBugService(QObject *parent) : QObject(parent) { }

void ReportBugService::sendBugReport(const BugData &data) {
    auto multipartCallable = [this, data]() {
        // Preparing Multipart Request
        QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        // Subject Part
        QHttpPart subPart;
        subPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"subject\""));
        subPart.setBody(data.subject.toUtf8());
        multipart->append(subPart);

        // Description Part
        QHttpPart descpPart;
        descpPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"description\""));
        descpPart.setBody(data.description.toUtf8());
        multipart->append(descpPart);

        // App Version
        QHttpPart appVersionPart;
        appVersionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"app_version\""));
        appVersionPart.setBody(APP_VERSION);
        multipart->append(appVersionPart);

        // Loading Screenshot
        if (!data.screenshotPath.isEmpty()) {
            QFile *file = new QFile(data.screenshotPath);
            file->setParent(multipart);

            if (!file->exists() || !file->open(QIODevice::ReadOnly)) {
                ERROR_HERE("Unable to load the screenshot file.";)
                delete file;
            }
        
            // Screenshot Info
            QFileInfo fileInfo(data.screenshotPath);
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
                emit reportSubmitted();
                return; 
            } else {
                // This will handle the server related error
                DEBUG_HERE(obj["status_code"].toString() + "   " + obj["message"].toString());
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
            emit noInternet();
        }
    );
}
