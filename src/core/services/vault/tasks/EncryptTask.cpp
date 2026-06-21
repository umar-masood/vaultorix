#include "EncryptTask.h"

#include "../../../config/Constants.h"
#include "../repository/FileRepository.h"
#include "../../../crypto/encryption/Encryption.h"
#include "../../../crypto/key/KeyManager.h"
#include "./TaskManager.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <openssl/rand.h>
#include <QCoreApplication>

using Core::Vault::EncryptTask;
using Core::Errors::EncryptError;

EncryptTask::EncryptTask(int fileId, QObject *parent)
           : QObject(parent), _fileId(fileId)
{
    setAutoDelete(true);

    // Fetching all metadata of current file from Database
    _metadata = Core::Vault::FileRepository::fetchFile(_fileId).value();
}

void EncryptTask::run() {
    // When application is about to close
    if (QCoreApplication::closingDown())
        return;

    // Source File (Imported Temp Files)
    QFile sourceFile(_metadata.tempPath);
    
    // Destination File
    QString basePath = VAULT_SECRET_PATH + "/encrypted/" + _metadata.tempName;
    QString tempDestFilePath = basePath  + ".vxenc.vxtemp";
    QString permanentDestFilePath = basePath + ".vxenc";

    QFile destFile(tempDestFilePath);

    // Encryption Failure Handler
    bool renamed = false;
    auto fail = [&](EncryptError error) {
        destFile.close();
        sourceFile.close();

        if (renamed)
            QFile::remove(permanentDestFilePath);
        else
        QFile::remove(tempDestFilePath);
        
        Core::Vault::FileRepository::updateFileStatus(_metadata.fileId, Core::Vault::FileStatus::Imported);
        emit statusChanged(_fileId, Core::Vault::FileStatus::Imported);
    };

    // Source File Opening
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        fail(EncryptError::SourceFileOpenFailed);
        return;
    }

    // Making necessary directories upto current point
    QFileInfo destInfo(tempDestFilePath);
    QDir destDir = destInfo.dir();

    if (!destDir.exists() && !destDir.mkpath(".")) {
        fail(EncryptError::DestinationFileCreateFailed);
        return;
    }

    // Destination File Opening
    if (!destFile.open(QIODevice::WriteOnly)) {
        fail(EncryptError::DestinationFileOpenFailed);
        return;
    }

    // Changing File Status to Encrypting in UI and Database
    emit statusChanged(_fileId, Core::Vault::FileStatus::Encrypting); 
    Core::Vault::FileRepository::updateFileStatus(_metadata.fileId, Core::Vault::FileStatus::Encrypting);

    // Encrypting File Metadata
    // Writing Header (Magic) to file
    if (destFile.write("VAULTORIXFILE", 14) != 14) {
        fail(EncryptError::DestinationFileWriteFailed);
        return;
    }

    // IV for Randomness
    QByteArray iv(12, Qt::Uninitialized); 
    if (RAND_bytes(
            reinterpret_cast<unsigned char *>(iv.data()),
            iv.size()
        ) != 1
    ) {
        fail(EncryptError::EncryptFailed);
        return;
    }

    // Writing iv to file
    if (destFile.write(iv, iv.size()) != iv.size()) {
        fail(EncryptError::DestinationFileWriteFailed);
        return;
    }

    // Generating Individual File Key
    QByteArray key = Core::Crypto::KeyManager::generateFileKey(); 

    // Init Encryption
    Core::Crypto::Encryption enc;
    enc.init(key, iv);

    // File Size
    const qint64 totalSize = _metadata.size;
    qint64 totalProcessedBytes = 0;

    while (!sourceFile.atEnd()) {
        // If application is shutting down
        if (TaskManager::shutdownRequested) {
            fail(EncryptError::EncryptFailed);
            return;
        }

        // Chunk of a file
        const QByteArray chunk = sourceFile.read(VAULT_ENCRYPT_CHUNK_SIZE);
        if (chunk.isEmpty() && sourceFile.error() != QFileDevice::NoError) {
            fail(EncryptError::SourceFileReadFailed);
            return;
        }

        // Encrypting Current Chunk
        QByteArray encryptedChunk = enc.encryptChunk(chunk);
        if (encryptedChunk.isEmpty()) {
            fail(EncryptError::EncryptFailed);
            return;
        }

        // Total Processed Bytes
        totalProcessedBytes += chunk.size();

        // Checking Writing Encrypted Chunk to dest File
        qint64 written = destFile.write(encryptedChunk);
        if (written != encryptedChunk.size()) {
            fail(EncryptError::DestinationFileWriteFailed);
            return;
        }

        // Calculate Progress Percent
        int percent = 0;
        if (totalSize > 0) {
            percent = static_cast<int>(
                (double(totalProcessedBytes) / double(totalSize)) * 100.0
            );
        }
        emit progressChanged(_fileId, percent);
    }

    enc.finish();

    // Writing Authentication Tag at the end of CipherText in dest File
    if (destFile.write(enc.tag()) != enc.tag().size()) {
        fail(EncryptError::DestinationFileWriteFailed);
        return;
    }

    // Flushing Dest File
    if (!destFile.flush()) {
        fail(EncryptError::EncryptFailed);
        return;
    }

    // Closing both files
    destFile.close();
    sourceFile.close();

    // Renaming Encrypted file from temp to permanent
    if (!QFile::rename(tempDestFilePath, permanentDestFilePath)) {
        fail(EncryptError::DestinationFileCreateFailed);
        return;
    }

    // When file is renamed to permanent dest file
    renamed = true;

    // Inserting encrypted file data in current metadata
    _metadata.encryptedName = _metadata.tempName;
    _metadata.encyptedPath  = permanentDestFilePath;

    // Encrypting File Key with Master Key
    auto data = Core::Crypto::KeyManager::instance()->wrapFileKey(key);

    // Validating Data
    if (data.encryptedKey.isEmpty() ||
        data.iv.isEmpty() ||
        data.tag.isEmpty()) 
    {
        fail(EncryptError::EncryptFailed);
        return;
    }

    // Inserting Encrypted Key IV, Tag, and key itself corresponding to current file in database
    if (!Core::Vault::FileRepository::insertEncryptedFileData(
        _metadata,
        data.encryptedKey,
        data.iv,
        data.tag
    )) {
        fail(EncryptError::EncryptFailed);
        return;
    }
    
    // Removing Imported Temp File Permanently
    QFile::remove(_metadata.tempPath);

    // Finalizing Progress to 100
    emit progressChanged(_fileId, 100);

    // Changing Status to Encrypted in UI and database
    emit statusChanged(_fileId, Core::Vault::FileStatus::Encrypted);
    Core::Vault::FileRepository::updateFileStatus(_metadata.fileId, Core::Vault::FileStatus::Encrypted);
}