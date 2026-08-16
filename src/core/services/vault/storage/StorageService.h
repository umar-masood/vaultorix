#pragma once

#include <QObject>
#include <QString>
#include <QUuid>
#include <QSet>

#include "../models/FileMetadata.h"
#include "../models/FileStatus.h"
#include "../../../errors/ImportError.h"

namespace Core::Vault {
    class StorageService : public QObject {
        Q_OBJECT
        
        public:
        explicit StorageService(QObject *parent = nullptr);
        
        bool isDocument(const QString &mime);
        bool isArchive(const QString &mime);
        bool isExecutable(const QString &mime);
        Core::Vault::FileType detectFileType(const QString &path);

        void importFile(const QString &source);
        void deleteFile(int fileId);
        void encryptFile(int fileId);
        void decryptFile(int fileId);
        void restoreFile(int fileId);
        
        private:
        // Mime Types
        const QSet<QString> documentMimeTypes = {
            "application/pdf",

            "application/msword",
            "application/vnd.openxmlformats-officedocument.wordprocessingml.document",

            "application/vnd.ms-excel",
            "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",

            "application/vnd.ms-powerpoint",
            "application/vnd.openxmlformats-officedocument.presentationml.presentation",

            "text/plain",
            "text/html",
            "text/csv", 
            "application/rtf"
        };

        const QSet<QString> archiveMimeTypes = {
            "application/zip",
            "application/x-7z-compressed",
            "application/x-rar",
            "application/x-tar",
            "application/gzip",
            "application/x-bzip2"
        };

        const QSet<QString> executableMimeTypes = {
            "application/x-dosexec",
            "application/x-msdownload",
            "application/x-msi",
            "application/x-executable",

            "application/vnd.android.package-archive",
            "application/vnd.debian.binary-package",

            "application/x-rpm",
            "application/x-apple-diskimage"
        };

        signals:
        void importQueued(const Core::Vault::FileMetadata &metadata);
        void importStatusChanged(int fileId, const Core::Vault::FileStatus &status);
        void importProgressChanged(int fileId, int progress);

        void deleteQueued(int fileId);
        void deleteStatusChanged(int fileId, const Core::Vault::FileStatus &status);
        void deleteProgressChanged(int fileId, int progress);

        void encryptQueued(int fileId);
        void encryptStatusChanged(int fileId, const Core::Vault::FileStatus &status);
        void encryptProgressChanged(int fileId, int progress);

        void decryptQueued(int fileId);
        void decryptStatusChanged(int fileId, const Core::Vault::FileStatus &status);
        void decryptProgressChanged(int fileId, int progress);

        void restoreQueued(int fileId);
        void restoreStatusChanged(int fileId, const Core::Vault::FileStatus &status);
        void restoreProgressChanged(int fileId, int progress);
    };
};
