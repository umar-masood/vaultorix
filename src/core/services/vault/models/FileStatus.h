#pragma once
#include <QMetaType>

namespace Core::Vault {
    enum class FileStatus {
        Importing,
        Encrypting,
        Decrypting,
        Deleting, 
        Restoring,
        
        Imported,
        Encrypted,
        Decrypted,
        Deleted,
        Restored,

        Queued,
        Unknown,
        Failed
    };
};

Q_DECLARE_METATYPE(Core::Vault::FileStatus)