#include "FileRepository.h"
#include "../database/Database.h"
#include "../../../config/Constants.h"

using Core::Vault::FileRepository;
using Core::Vault::Database;
using Core::Vault::FileMetadata;

std::optional<int> FileRepository::insertFile(const FileMetadata &fileMetadata) {
    QSqlQuery query(Database::instance().database());
    query.prepare(
        R"(
            INSERT INTO FILES (
                actual_name, temp_name, extension, type, original_path, temp_path, size, created_at, status
            )
            VALUES (
                :actual_name, :temp_name, :extension, :type, :original_path, :temp_path, :size, :created_at, :status
            )
            RETURNING id
        )"
    );

    query.bindValue(":actual_name",     fileMetadata.actualName);
    query.bindValue(":temp_name",       fileMetadata.tempName);
    query.bindValue(":extension",       fileMetadata.extension);
    query.bindValue(":type",            static_cast<int>(fileMetadata.type));
    query.bindValue(":original_path",   fileMetadata.originalPath);
    query.bindValue(":temp_path",       fileMetadata.tempPath);
    query.bindValue(":size",            fileMetadata.size);
    query.bindValue(":created_at",      fileMetadata.createdAt);
    query.bindValue(":status",          static_cast<int>(fileMetadata.status));

    if (!query.exec()) 
        ERROR_HERE("Failed to insert file record in database: " + query.lastError().text());
    
    if (!query.next()) 
        return std::nullopt;

    return query.value("id").toInt();
}

void FileRepository::removeFile(int fileId) {
    QSqlQuery query(Database::instance().database());
    query.prepare(
        R"(
            DELETE FROM files
            WHERE id = :id
        )"
    );

    query.bindValue(":id", fileId);

    if (!query.exec())
        ERROR_HERE("Failed to remove file record from database: " + query.lastError().text());
}

void FileRepository::updateFileStatus(int fileId, FileStatus currentStatus) {
    QSqlQuery query(Database::instance().database());
    query.prepare(
        R"(
            UPDATE files
            SET status = :status
            WHERE id = :id
        )"
    );

    query.bindValue(":id", fileId);
    query.bindValue(":status", static_cast<int>(currentStatus));

    if (!query.exec())
        ERROR_HERE("Failed to update file record in database: " + query.lastError().text());
}

Core::Vault::FileStatus FileRepository::fetchFileStatus(int fileId) {
    QSqlQuery query(Database::instance().database());

    query.prepare(
        R"(
            SELECT status
            FROM files
            WHERE id = :id
        )"
    );

    query.bindValue(":id", fileId);

    if (!query.exec()) {
        ERROR_HERE("Failed to fetch file status from database: "+ query.lastError().text());
        return FileStatus::Unknown;
    }

    if (!query.next()) 
        return FileStatus::Unknown;

    return static_cast<FileStatus>(
        query.value(0).toInt()
    );
}

FileMetadata FileRepository::mapQueryToFileMetadata(QSqlQuery &query) {
    FileMetadata metadata;

    metadata.fileId = query.value("id").toInt();
    metadata.actualName = query.value("actual_name").toString();
    metadata.tempName = query.value("temp_name").toString();
    metadata.extension = query.value("extension").toString();
    metadata.type = static_cast<FileType>(query.value("type").toInt());
    metadata.originalPath = query.value("original_path").toString();
    metadata.tempPath = query.value("temp_path").toString();
    metadata.size = query.value("size").toLongLong();
    metadata.createdAt = query.value("created_at").toDateTime();
    metadata.status = static_cast<FileStatus>(query.value("status").toInt());
    metadata.encryptedName = query.value("encrypted_name").toString();
    metadata.encyptedPath = query.value("encrypted_path").toString();
    metadata.decryptedPath = query.value("decrypted_path").toString();

    return metadata;
}

std::optional<Core::Vault::FileMetadata> FileRepository::fetchFile(int fileId) {
    QSqlQuery query(Database::instance().database());

    query.prepare(
        R"(
            SELECT *
            FROM files
            WHERE id = :id
        )"
    );

    query.bindValue(":id", fileId);

    if (!query.exec()) {
        ERROR_HERE("Failed to fetch file from database: " + query.lastError().text());
        return std::nullopt;
    }

    if (!query.next()) {
        return std::nullopt;
    }

    return mapQueryToFileMetadata(query);
}

std::optional<QList<FileMetadata>> FileRepository::fetchFiles() {
    QSqlQuery query(Database::instance().database());

    query.prepare(
        R"(
            SELECT *
            FROM files
            ORDER BY id DESC
        )"
    );

    if (!query.exec()) {
        ERROR_HERE( "Failed to fetch files from database: " + query.lastError().text()
        );
        return std::nullopt;
    }

    QList<FileMetadata> files;

    while (query.next())
        files.append(mapQueryToFileMetadata(query));

    return files;
}

void FileRepository::markImportingFilesAsFailed() {
    QSqlQuery query(Database::instance().database());

    query.prepare(
        R"(
            UPDATE files
            SET status = :failed
            WHERE status = :importing
        )"
    );

    query.bindValue(":failed", static_cast<int>(Core::Vault::FileStatus::Failed));
    query.bindValue(":importing", static_cast<int>(Core::Vault::FileStatus::Importing));

    if (!query.exec())
        ERROR_HERE("Failed to mark importing files as failed: " + query.lastError().text());
}

void FileRepository::markEncryptingFilesImported() {
    QSqlQuery query(Database::instance().database());

    query.prepare(
        R"(
            UPDATE files
            SET status = :imported
            WHERE status = :encrypting
        )"
    );

    query.bindValue(":imported", static_cast<int>(Core::Vault::FileStatus::Imported));
    query.bindValue(":encrypting", static_cast<int>(Core::Vault::FileStatus::Encrypting));

    if (!query.exec())
        ERROR_HERE("Failed to mark encrypting files as imported: " + query.lastError().text());
}

void FileRepository::markDecryptingFilesEncrypted() {
    QSqlQuery query(Database::instance().database());

    query.prepare(
        R"(
            UPDATE files
            SET status = :encrypted
            WHERE status = :decrypting
        )"
    );

    query.bindValue(":encrypted", static_cast<int>(Core::Vault::FileStatus::Encrypted));
    query.bindValue(":decrypting", static_cast<int>(Core::Vault::FileStatus::Decrypting));

    if (!query.exec())
        ERROR_HERE("Failed to mark decrypting files as encrypted: " + query.lastError().text());
}

bool FileRepository::insertEncryptedFileData(const Core::Vault::FileMetadata &metadata, 
                                            const QByteArray &key,
                                            const QByteArray &iv,
                                            const QByteArray &tag)
{
    QSqlQuery query(Database::instance().database());
    query.prepare(
        R"(
            UPDATE files
            SET temp_name       = NULL,
                temp_path       = NULL,
                encrypted_name  = :encName,
                encrypted_path  = :encPath,
                key             = :key,
                iv              = :iv,
                tag             = :tag
            WHERE id = :id
        )"
    );

    query.bindValue(":id", metadata.fileId);
    query.bindValue(":encName", metadata.encryptedName);
    query.bindValue(":encPath", metadata.encyptedPath);
    query.bindValue(":key", key);
    query.bindValue(":iv", iv);
    query.bindValue(":tag", tag);

    if (!query.exec()) {
        ERROR_HERE("Failed to insert encrypted file data in database: " + query.lastError().text());
        return false;
    }

    return true;
}

std::optional<Core::Crypto::WrappedKey> FileRepository::fetchEncryptedFileSecrets(int fileId) {
    QSqlQuery query(Database::instance().database());
    query.prepare(
        R"(
            SELECT
            key, iv, tag
            FROM files
            WHERE id = :id
        )"
    );

    query.bindValue(":id", fileId);

    if (!query.exec()) {
        ERROR_HERE("Failed to fetch encryption scerets of file from database: " + query.lastError().text());
        return std::nullopt;
    }

    if (!query.next()) {
        return std::nullopt;
    }

    Core::Crypto::WrappedKey key;
    key.encryptedKey = query.value("key").toByteArray();
    key.iv           = query.value("iv").toByteArray();
    key.tag          = query.value("tag").toByteArray();

    return key;
}
