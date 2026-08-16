#pragma once

namespace Core::Errors {
    enum class ImportError {
        InvalidSourcePath,
        SourceFileMissing,
        SourceFileOpenFailed,
        SourceFileReadFailed,
        TempFileCreateFailed,
        TempFileWriteFailed,
        FileTooLarge,
        MetadataExtractionFailed,
        EncryptionFailed,
        DatabaseSaveFailed,
    };
};