#include "DeleteTask.h"
#include "../repository/FileRepository.h"
#include "../../../config/Constants.h"

#include <QFile>

using Core::Vault::DeleteTask;
DeleteTask::DeleteTask(int fileId, QObject *parent) : QObject(parent), _fileId(fileId) {
    setAutoDelete(true);
}

void DeleteTask::run() {
    emit statusChanged(_fileId, Core::Vault::FileStatus::Deleting);

    auto metadata = Core::Vault::FileRepository::fetchFile(_fileId).value();
    auto status = metadata.status;

    QString _filePath;
    if (status == Core::Vault::FileStatus::Decrypted) 
        _filePath = metadata.decryptedPath;
    else
        _filePath = metadata.tempPath;

    QFile file(_filePath);
    if (!file.exists()) {
        ERROR_HERE("Failed to delete the file permanently.");
        return;
    }

    emit progressChanged(_fileId, 50);

    if (status == Core::Vault::FileStatus::Decrypted) {
        QFile::remove(metadata.decryptedPath);
        QFile::remove(metadata.encyptedPath);
    } else {
        QFile::remove(_filePath);
    }
    
    Core::Vault::FileRepository::removeFile(_fileId);

    emit progressChanged(_fileId, 100);
    emit statusChanged(_fileId, Core::Vault::FileStatus::Deleted);
}
