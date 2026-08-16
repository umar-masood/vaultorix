#include "DecryptTask.h"

#include "../../../config/Constants.h"
#include "../repository/FileRepository.h"
#include "../../../crypto/decryption/Decryption.h"
#include "../../../crypto/key/KeyManager.h"
#include "./TaskManager.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <openssl/rand.h>
#include <QCoreApplication>

using Core::Vault::DecryptTask;
using Core::Errors::DecryptError;

DecryptTask::DecryptTask(int fileId, QObject *parent)
           : QObject(parent), _fileId(fileId)
{
    setAutoDelete(true);

    // Fetching all metadata of current file from Database
    _metadata = Core::Vault::FileRepository::fetchFile(_fileId).value();
}

void DecryptTask::run() {
    // When application is about to close
    if (QCoreApplication::closingDown())
        return;

    // Source File (Encrypted Files)
    QFile sourceFile(_metadata.encyptedPath);
    
    // Destination File
    QString basePath = VAULT_SECRET_PATH + "/decrypted/" + _metadata.encryptedName;
    QString tempDestFilePath = basePath  + ".vxdec.vxtemp";
    QString permanentDestFilePath = basePath + "." + _metadata.extension.toLower();

    QFile destFile(tempDestFilePath);

    // Decryption Failure Handler
    bool renamed = false;
    auto fail = [&](DecryptError error) {
        destFile.close();
        sourceFile.close();

        if (renamed)
            QFile::remove(permanentDestFilePath);
        else
            QFile::remove(tempDestFilePath);
        
        Core::Vault::FileRepository::updateFileStatus(_metadata.fileId, Core::Vault::FileStatus::Encrypted);
        emit statusChanged(_fileId, Core::Vault::FileStatus::Encrypted);
    };

    // Source File Opening
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        fail(DecryptError::SourceFileOpenFailed);
        return;
    }

    // Making necessary directories upto current point
    QFileInfo destInfo(tempDestFilePath);
    QDir destDir = destInfo.dir();

    if (!destDir.exists() && !destDir.mkpath(".")) {
        fail(DecryptError::DestinationFileCreateFailed);
        return;
    }

    // Destination File Opening
    if (!destFile.open(QIODevice::WriteOnly)) {
        fail(DecryptError::DestinationFileOpenFailed);
        return;
    }

    // Changing File Status to Decrypting in UI and Database
    emit statusChanged(_fileId, Core::Vault::FileStatus::Decrypting); 
    Core::Vault::FileRepository::updateFileStatus(_metadata.fileId, Core::Vault::FileStatus::Decrypting);

    // Decrypting File
    // Verifying Header to ensure the file is encrypted by Vaultorix
    QByteArray header = sourceFile.read(14);
    if (!header.contains("VAULTORIXFILE")) {
        fail(DecryptError::DecryptFailed);
        return;
    }

    // Fetching WrappedKey from database for this file
    using FR = Core::Vault::FileRepository;
    auto wrappedKeyOpt = FR::fetchEncryptedFileSecrets(_fileId);
    if (!wrappedKeyOpt.has_value()) {
        fail(DecryptError::DecryptFailed);
        return;
    }

    Core::Crypto::WrappedKey wrappedKey = *wrappedKeyOpt;

    using KM = Core::Crypto::KeyManager;
    auto decryptionKeyOpt = KM::instance()->unwrapFileKey(wrappedKey);
    if (!decryptionKeyOpt.has_value()) {
        fail(DecryptError::DecryptFailed);
        return;
    }

    QByteArray decryptionKey = *decryptionKeyOpt;

    // Extracting IV from file
    QByteArray iv = sourceFile.read(12);

    // Init Decryption
    Core::Crypto::Decryption dec;
    dec.init(decryptionKey, iv, wrappedKey.tag);

    // File Size
    const qint64 totalSize = _metadata.size;
    qint64 totalProcessedBytes = 0;

    while (!sourceFile.atEnd()) {
        // If application is shutting down
        if (TaskManager::shutdownRequested) {
            fail(DecryptError::DecryptFailed);
            return;
        }

        // Chunk of a file
        const QByteArray chunk = sourceFile.read(VAULT_ENCRYPT_CHUNK_SIZE);
        if (chunk.isEmpty() && sourceFile.error() != QFileDevice::NoError) {
            fail(DecryptError::SourceFileReadFailed);
            return;
        }

        // Decrypting Current Chunk
        QByteArray decryptedChunk = dec.decryptChunk(chunk);
        if (decryptedChunk.isEmpty()) {
            fail(DecryptError::DecryptFailed);
            return;
        }

        // Total Processed Bytes
        totalProcessedBytes += chunk.size();

        // Checking Written Decrypted Chunk to dest File
        qint64 written = destFile.write(decryptedChunk);
        if (written != decryptedChunk.size()) {
            fail(DecryptError::DestinationFileWriteFailed);
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

    dec.finish();

    // Flushing Dest File
    if (!destFile.flush()) {
        fail(DecryptError::DecryptFailed);
        return;
    }

    // Closing both files
    destFile.close();
    sourceFile.close();

    // Renaming Decrypted file from temp to permanent
    if (!QFile::rename(tempDestFilePath, permanentDestFilePath)) {
        fail(DecryptError::DestinationFileCreateFailed);
        return;
    }

    // When file is renamed to permanent dest file
    renamed = true;

    // Adding decrypted file path to DB
    if (!Core::Vault::FileRepository::setFileDecryptedNamePath(_fileId, permanentDestFilePath, _metadata.encryptedName)) {
        fail(DecryptError::DecryptFailed);
        return;
    }

    // Deleting Encrypted File
    if (!Core::Vault::FileRepository::removeEncryptedFileNamePath(_fileId)) {
        fail(DecryptError::DecryptFailed);
        return;
    }
    
    QFile::remove(_metadata.encyptedPath);

    // Finalizing Progress to 100
    emit progressChanged(_fileId, 100);

    // Changing Status to Decrypted in UI and database
    emit statusChanged(_fileId, Core::Vault::FileStatus::Decrypted);
    Core::Vault::FileRepository::updateFileStatus(_metadata.fileId, Core::Vault::FileStatus::Decrypted);
}