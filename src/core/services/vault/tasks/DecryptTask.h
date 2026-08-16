#pragma once

#include <QRunnable>
#include <QObject>
#include <QThreadPool>

#include "../../../errors/DecryptError.h"
#include "../models/FileStatus.h"
#include "../models/FileMetadata.h"

namespace Core::Vault {
    class DecryptTask : public QObject, public QRunnable {
        Q_OBJECT

        public:
        DecryptTask(int fileId, QObject *parent = nullptr);
        void run() override;

        private:
        int _fileId;
        FileMetadata _metadata;

        signals:
        void statusChanged(int fileId, Core::Vault::FileStatus status);
        void progressChanged(int fileId, int progress);
    };
};