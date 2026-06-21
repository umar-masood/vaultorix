#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace Core::Vault {
    class Database : public QObject {

        public:
        static Database& instance();
        QSqlDatabase database();
        void closeCurrentThreadDatabaseConnection();

        private:
        explicit Database(QObject *parent = nullptr);

        QString filesTableCreateQuery();
        void createFilesTable(const QSqlDatabase &db);
        QString connectionName();
    };
};  