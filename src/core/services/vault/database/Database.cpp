#include "Database.h"
#include "../../../config/Constants.h"

#include <QDir>
#include <QThread>

using Core::Vault::Database;

Database& Database::instance() {
    static Database instance;
    return instance;
}

Database::Database(QObject *parent) : QObject(parent) {
    QDir dir(VAULT_SECRET_PATH + "/config");
    dir.mkpath(".");
}

QString Database::filesTableCreateQuery() {
    return QString(R"(
        CREATE TABLE IF NOT EXISTS files (
            id                      INTEGER PRIMARY KEY AUTOINCREMENT,
            actual_name             TEXT NOT NULL,
            temp_name               TEXT,
            encrypted_name          TEXT DEFAULT NULL,
            type                    TEXT NOT NULL,
            extension               TEXT NOT NULL,
            original_path           TEXT NOT NULL,
            temp_path               TEXT,
            encrypted_path          TEXT DEFAULT NULL,
            decrypted_name          TEXT DEFAULT NULL,
            decrypted_path          TEXT DEFAULT NULL,
            key                     TEXT DEFAULT NULL,
            iv                      TEXT DEFAULT NULL,
            tag                     TEXT DEFAULT NULL,
            size                    BIGINT NOT NULL,
            created_at              TEXT NOT NULL,
            status                  INTEGER NOT NULL
        )
    )");
}

void Database::createFilesTable(const QSqlDatabase &db) {
    QSqlQuery query(db);
    if (!query.exec(filesTableCreateQuery()))
        ERROR_HERE("Failed to create files relation inside database: " + query.lastError().text());
}

QString Database::connectionName() {
    return QString("vault_connection_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
}

QSqlDatabase Database::database() {
    if (QSqlDatabase::contains(connectionName()))
        return QSqlDatabase::database(connectionName());

    QSqlDatabase _db = QSqlDatabase::addDatabase("QSQLITE", connectionName());
    _db.setDatabaseName(VAULT_SECRET_PATH + "/config/data.vxmeta");

    if (!_db.open())  {
        ERROR_HERE("Failed to open metadata database " + _db.lastError().text());
        return QSqlDatabase(); // later we will fix it
    }

    createFilesTable(_db);

    return _db;
}

void Database::closeCurrentThreadDatabaseConnection() {
    if (QSqlDatabase::contains(connectionName())) {
        QSqlDatabase _db = QSqlDatabase::database(connectionName());

        if (_db.isOpen())
            _db.close();

        QSqlDatabase::removeDatabase(connectionName());
    }
}