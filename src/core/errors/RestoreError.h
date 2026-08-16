#pragma once

namespace Core::Errors {
    enum class RestoreError {
        InvalidSourcePath,
        SourceFileMissing,
        SourceFileOpenFailed,
        SourceFileReadFailed,
        DestinationFileCreateFailed,
        DestinationFileWriteFailed,
        RestoreFailed,
        DestinationFileOpenFailed
    };
};