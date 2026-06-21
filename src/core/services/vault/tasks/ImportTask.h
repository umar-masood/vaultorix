#pragma once

#include <QObject>
#include <QUuid>
#include <QRunnable>
#include <QThreadPool>

#include "../models/FileMetadata.h"
#include "../../../errors/ImportError.h"

namespace Core::Vault {
    class ImportTask : public QObject, public QRunnable {
        Q_OBJECT

        public:
        explicit ImportTask(const FileMetadata &metadata,
                            QObject *parent = nullptr);

        void run() override;

        signals:
        void statusChanged(int fileId, Core::Vault::FileStatus status);
        void progressChanged(int fileId, int progress);

        private:
        int _fileId;
        FileMetadata _metadata;
    };
}