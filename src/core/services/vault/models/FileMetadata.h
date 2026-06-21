#pragma once

#include <QString>
#include <QDateTime>

#include "FileType.h"
#include "FileStatus.h"

namespace Core::Vault {
    struct FileMetadata {
        int fileId = -1;
        
        QString actualName;
        QString tempName        =   QString();
        QString encryptedName   =   QString();
        
        QString extension;
        Core::Vault::FileType type;
        
        QString originalPath    =   QString();
        QString tempPath        =   QString();
        QString encyptedPath    =   QString();
        QString decryptedPath   =   QString();

        qint64  size = 0;
        QDateTime createdAt;
        Core::Vault::FileStatus status = Core::Vault::FileStatus::Queued;
    };
};  
