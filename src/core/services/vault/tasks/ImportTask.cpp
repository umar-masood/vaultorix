#include "ImportTask.h"
#include "../../../config/Constants.h"
#include "../repository/FileRepository.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>

using Core::Vault::ImportTask;
using Core::Errors::ImportError;

ImportTask::ImportTask(const Core::Vault::FileMetadata &metadata, QObject *parent)
           : QObject(parent), _fileId(metadata.fileId), _metadata(metadata)
{
    setAutoDelete(true);
}

void ImportTask::run() {
    QFile sourceFile(_metadata.originalPath);
    QFile destFile(_metadata.tempPath);

    auto fail = [&](ImportError error, bool removeTempFile = true) {
        destFile.close();
        sourceFile.close();

        if (removeTempFile) 
            destFile.remove();

        Core::Vault::FileRepository::updateFileStatus(_metadata.fileId, Core::Vault::FileStatus::Failed);
        emit statusChanged(_fileId, Core::Vault::FileStatus::Failed);
    };

    if (!sourceFile.open(QIODevice::ReadOnly)) {
        fail(ImportError::SourceFileOpenFailed, false);
        return;
    }

    QFileInfo destInfo(_metadata.tempPath);
    QDir destDir = destInfo.dir();

    if (!destDir.exists() && !destDir.mkpath(".")) {
        fail(ImportError::TempFileCreateFailed, false);
        return;
    }

    if (!destFile.open(QIODevice::WriteOnly)) {
        fail(ImportError::TempFileCreateFailed, false);
        return;
    }

    emit statusChanged(_fileId, Core::Vault::FileStatus::Importing);
    Core::Vault::FileRepository::updateFileStatus(_metadata.fileId, Core::Vault::FileStatus::Importing);

    const qint64 totalSize = _metadata.size;
    qint64 totalReadSize = 0;

    while (!sourceFile.atEnd()) {
        const QByteArray chunk = sourceFile.read(VAULT_IMPORT_CHUNK_SIZE);

        if (chunk.isEmpty() && sourceFile.error() != QFileDevice::NoError) {
            fail(ImportError::SourceFileReadFailed);
            return;
        }

        const qint64 written = destFile.write(chunk);

        if (written != chunk.size()) {
            fail(ImportError::TempFileWriteFailed);
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
        fail(ImportError::TempFileWriteFailed);
        return;
    }

    QFileInfo destFileInfo(destFile);

    if (_metadata.size != destFileInfo.size()) {
        fail(ImportError::TempFileWriteFailed);
        return;
    }

    destFile.close();
    sourceFile.close();

    emit statusChanged(_fileId, Core::Vault::FileStatus::Imported);
    Core::Vault::FileRepository::updateFileStatus(_metadata.fileId, Core::Vault::FileStatus::Imported);
    emit progressChanged(_fileId, 100);
}