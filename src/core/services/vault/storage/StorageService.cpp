#include "StorageService.h"

#include "../repository/FileRepository.h"
#include "../../../config/Constants.h"

#include "../tasks/ImportTask.h"
#include "../tasks/DeleteTask.h"
#include "../tasks/EncryptTask.h"
#include "../tasks/DecryptTask.h"
#include "../tasks/RestoreTask.h"

#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QDir>
#include <QThread>
#include <QMimeDatabase>

using Core::Errors::ImportError;
using Core::Vault::StorageService;

StorageService::StorageService(QObject *parent) : QObject(parent) {
    QThreadPool::globalInstance()->setMaxThreadCount(2);
}

bool StorageService::isDocument(const QString &mime) {
    return documentMimeTypes.contains(mime);
}

bool StorageService::isArchive(const QString &mime) {
    return archiveMimeTypes.contains(mime);
}

bool StorageService::isExecutable(const QString &mime) {
    return executableMimeTypes.contains(mime);
}

Core::Vault::FileType StorageService::detectFileType(const QString &path) {
    QMimeDatabase db;
    QString mime = db.mimeTypeForFile(path, QMimeDatabase::MatchDefault).name();

    if (mime.startsWith("image/")) 
        return Core::Vault::FileType::Picture;

    if (mime.startsWith("video/"))
        return Core::Vault::FileType::Video;

    if (mime.startsWith("audio/"))
        return Core::Vault::FileType::Audio;

    if (isDocument(mime))
        return Core::Vault::FileType::Document;

    if (isArchive(mime))
        return Core::Vault::FileType::Archive;

    if (isExecutable(mime))
        return FileType::Application;

    return Core::Vault::FileType::Other;
}

void StorageService::importFile(const QString &source) {
    QUuid _jobId = QUuid::createUuid();

    if (source.isEmpty()) {
        WARN_HERE("Source file path is empty.");
        return;
    }

    // File Information
    QFileInfo info(source);
    qint64 totalFileSize = info.size();

    if (!info.exists() || !info.isFile()) {
        ERROR_HERE("The source file does not exist.");
        return;
    }

    // Checking import file size
    if (totalFileSize > MAX_IMPORT_FILE_SIZE) {
        WARN_HERE("Vault file size is too large to be import.");
        return;
    }

    // Preparing file record for ui and database
    Core::Vault::FileMetadata metadata;
    metadata.actualName = info.completeBaseName();
    metadata.originalPath = source;

    QString _tempName = QUuid::createUuid().toString(QUuid::WithoutBraces);

    metadata.tempName   = _tempName;
    metadata.tempPath   = VAULT_SECRET_PATH + "/import/" + _tempName + ".vxtemp";
    metadata.extension  = info.suffix().toUpper();
    metadata.createdAt  = QDateTime::currentDateTime();
    metadata.status     = Core::Vault::FileStatus::Queued;
    metadata.size       = totalFileSize;
    metadata.type       = detectFileType(source);
    metadata.fileId     = Core::Vault::FileRepository::insertFile(metadata).value();

    emit importQueued(metadata);

    // Running in a thread
    auto *task = new Core::Vault::ImportTask(metadata);
    connect(task, &Core::Vault::ImportTask::statusChanged, this, &StorageService::importStatusChanged);
    connect(task, &Core::Vault::ImportTask::progressChanged, this, &StorageService::importProgressChanged);
    QThreadPool::globalInstance()->start(task);
}

void StorageService::deleteFile(int fileId) {
    // Queue File
    emit deleteQueued(fileId);

    // Running in a thread 
    auto *task = new Core::Vault::DeleteTask(fileId);
    connect(task, &Core::Vault::DeleteTask::statusChanged, this, &StorageService::deleteStatusChanged);
    connect(task, &Core::Vault::DeleteTask::progressChanged, this, &StorageService::deleteProgressChanged);
    QThreadPool::globalInstance()->start(task);
}

void StorageService::encryptFile(int fileId) {
    // Queue File 
    emit encryptQueued(fileId);

    // Running in a thread
    auto *task = new Core::Vault::EncryptTask(fileId);
    connect(task, &Core::Vault::EncryptTask::statusChanged, this, &StorageService::encryptStatusChanged);
    connect(task, &Core::Vault::EncryptTask::progressChanged, this, &StorageService::encryptProgressChanged);
    QThreadPool::globalInstance()->start(task);
}

void StorageService::decryptFile(int fileId) {
    // Queue File 
    emit decryptQueued(fileId);

    // Running in a thread
    auto *task = new Core::Vault::DecryptTask(fileId);
    connect(task, &Core::Vault::DecryptTask::statusChanged, this, &StorageService::decryptStatusChanged);
    connect(task, &Core::Vault::DecryptTask::progressChanged, this, &StorageService::decryptProgressChanged);
    QThreadPool::globalInstance()->start(task);
}

void StorageService::restoreFile(int fileId) {
    // Queue File 
    emit restoreQueued(fileId);

    // Running in a thread
    auto *task = new Core::Vault::RestoreTask(fileId);
    connect(task, &Core::Vault::RestoreTask::statusChanged, this, &StorageService::restoreStatusChanged);
    connect(task, &Core::Vault::RestoreTask::progressChanged, this, &StorageService::restoreProgressChanged);
    QThreadPool::globalInstance()->start(task);
}