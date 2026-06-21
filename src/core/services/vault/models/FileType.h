#pragma once

#include <QMetaType>

namespace Core::Vault {
    enum class FileType {
        Document,
        Archive,
        Video,
        Audio,
        Application,
        Picture,
        Other
    };
};

Q_DECLARE_METATYPE(Core::Vault::FileType);