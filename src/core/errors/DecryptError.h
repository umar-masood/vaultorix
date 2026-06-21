#pragma once

namespace Core::Errors {
    enum class DecryptError {
        DecryptFailed,
        SourceFileOpenFailed,
        DestinationFileOpenFailed,
        DestinationFileCreateFailed,
        SourceFileReadFailed,
        DestinationFileWriteFailed
    };
}