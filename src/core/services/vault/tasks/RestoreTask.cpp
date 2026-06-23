#include "RestoreTask.h"
#include "../repository/FileRepository.h"
#include "../../../config/Constants.h"
#include "../../../errors/RestoreError.h"

#include <QFile>
#include <QDir>

using Core::Vault::RestoreTask;
using Core::Errors::RestoreError;

RestoreTask::RestoreTask(int fileId, QObject *parent) : QObject(parent), _fileId(fileId) {
    setAutoDelete(true);

    // Fetching all metadata of current file from Database
    _metadata = Core::Vault::FileRepository::fetchFile(_fileId).value();
}

void RestoreTask::run() {
    QString sourcePath;

    auto status = _metadata.status;
    if (status == Core::Vault::FileStatus::Imported)
        sourcePath = _metadata.tempPath;
    else 
        sourcePath = _metadata.decryptedPath;

    qDebug() << sourcePath;

    QFile sourceFile(sourcePath);

    QString destPath = _metadata.originalPath;
    QFile destFile(destPath);

    qDebug() << destPath;

    QFileInfo destInfo(destPath);
    QDir destDir = destInfo.dir();

    auto fail = [&](RestoreError error) {
        destFile.close();
        sourceFile.close();

        Core::Vault::FileRepository::updateFileStatus(_metadata.fileId, Core::Vault::FileStatus::Failed);
        emit statusChanged(_fileId, Core::Vault::FileStatus::Failed);
    };

    if (!destDir.exists() && !destDir.mkpath(".")) {
        fail(RestoreError::DestinationFileCreateFailed);
        return;
    }
   
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        fail(RestoreError::SourceFileOpenFailed);
        return;
    }

    if (!destFile.open(QIODevice::WriteOnly)) {
        fail(RestoreError::DestinationFileOpenFailed);
        return;
    }

    emit statusChanged(_fileId, Core::Vault::FileStatus::Restoring);
    Core::Vault::FileRepository::updateFileStatus(_metadata.fileId, Core::Vault::FileStatus::Restoring);

    const qint64 totalSize = _metadata.size;
    qint64 totalReadSize = 0;

    while (!sourceFile.atEnd()) {
        const QByteArray chunk = sourceFile.read(VAULT_IMPORT_CHUNK_SIZE);

        if (chunk.isEmpty() && sourceFile.error() != QFileDevice::NoError) {
            fail(RestoreError::SourceFileReadFailed);
            return;
        }

        const qint64 written = destFile.write(chunk);
        if (written != chunk.size()) {
            fail(RestoreError::RestoreFailed);
            return;
        }

        totalReadSize += chunk.size();

        int percent = 0;
        if (totalSize > 0) {
            percent = static_cast<int>(
                (double(totalReadSize) / double(totalSize)) * 100.0
            );
        }

        emit progressChanged(_fileId, percent);
    }

    if (!destFile.flush()) {
        fail(RestoreError::RestoreFailed);
        return;
    }

    // QFileInfo destFileInfo(destFile);
    // if (_metadata.size != destFileInfo.size()) {
    //     fail(RestoreError::RestoreFailed);
    //     return;
    // }

    destFile.close();
    sourceFile.close();

    Core::Vault::FileRepository::removeFile(_metadata.fileId);
    qDebug() << _metadata.decryptedPath << "  AND   " << _metadata.tempPath;
    status == Core::Vault::FileStatus::Decrypted ? QFile::remove(_metadata.decryptedPath) : QFile::remove(_metadata.tempPath);
    
    emit progressChanged(_fileId, 100);
    emit statusChanged(_fileId, Core::Vault::FileStatus::Restored);
}