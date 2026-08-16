#pragma once

#include "../models/FileMetadata.h"
#include "../models/FileStatus.h"
#include "../../../crypto/key/KeyManager.h"

#include <QList>
#include <optional>
#include <QByteArray>

class QSqlQuery;

namespace Core::Vault {
    class FileRepository {

        public:    
        static std::optional<int> insertFile(const Core::Vault::FileMetadata &fileMetadata);
        static void removeFile(int fileId);
        static void updateFileStatus(int fileId, Core::Vault::FileStatus currentStatus); 
        static std::optional<Core::Vault::FileMetadata> fetchFile(int fileId);
        static std::optional<QList<Core::Vault::FileMetadata>> fetchFiles();
        static void markImportingFilesAsFailed();
        static void markEncryptingFilesImported();
        static void markDecryptingFilesEncrypted();
        static bool insertEncryptedFileData(const Core::Vault::FileMetadata &metadata,
                                            const QByteArray &key,
                                            const QByteArray &iv,
                                            const QByteArray &tag);
        static std::optional<Core::Crypto::WrappedKey> fetchEncryptedFileSecrets(int fileId);
        static bool setFileDecryptedNamePath(int fileId, const QString &decryptedPath, const QString &decryptedName);
        static bool removeEncryptedFileNamePath(int fileId);

        private:
        static Core::Vault::FileStatus fetchFileStatus(int fileId);
        static Core::Vault::FileMetadata mapQueryToFileMetadata(QSqlQuery &query);
    };
};