#pragma once

#include <QRunnable>
#include <QObject>

#include "../../../errors/DeleteError.h"
#include "../models/FileStatus.h"

namespace Core::Vault {
    class DeleteTask : public QObject, public QRunnable {
        Q_OBJECT

        public:
        DeleteTask(int fileId, QObject *parent = nullptr);
        void run() override;

        private:
        int _fileId;

        signals:
        void statusChanged(int fileId, Core::Vault::FileStatus status);
        void progressChanged(int fileId, int progress);
    };
};