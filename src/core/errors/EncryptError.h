#pragma once

namespace Core::Errors {
    enum class EncryptError {
        EncryptFailed,
        SourceFileOpenFailed,
        DestinationFileOpenFailed,
        DestinationFileCreateFailed,
        SourceFileReadFailed,
        DestinationFileWriteFailed
    };
}