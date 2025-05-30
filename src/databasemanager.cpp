#include "databasemanager.h"

void DatabaseManager::initialize()
{
    QSqlDatabase db = getDatabase();

    if (!db.open()) {
        qDebug() << "Failed to open database for initialization";
        return;
    }

    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS data (key TEXT PRIMARY KEY, value TEXT)");
    if (!query.exec()) {
        qDebug() << "Failed to create table:" << query.lastError().text();
    }

    qDebug() << "Database probably ok";

    db.close();
}

QString DatabaseManager::get(const QString &key)
{
    QSqlDatabase db = getDatabase();

    if (!db.open()) {
        qDebug() << "Failed to open database for get operation";
        return QString();
    }

    QSqlQuery query;
    query.prepare("SELECT value FROM data WHERE key = :key");
    query.bindValue(":key", key);

    if (!query.exec()) {
        qDebug() << "Failed to execute get query:" << query.lastError().text();
        db.close();
        return QString();
    }

    QString value;
    if (query.next()) {
        value = query.value(0).toString();
    } else {
        qDebug() << "No value found for key:" << key;
    }

    db.close();
    return value;
}

bool DatabaseManager::set(const QString &key, const QString &value)
{
    QSqlDatabase db = getDatabase();

    if (!db.open()) {
        qDebug() << "Failed to open database for set operation";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO data (key, value) VALUES (:key, :value)");
    query.bindValue(":key", key);
    query.bindValue(":value", value);

    bool success = query.exec();
    if (!success) {
        qDebug() << "Failed to execute set query:" << query.lastError().text();
    }

    db.close();
    return success;
}

QSqlDatabase DatabaseManager::getDatabase()
{
        //TEMP: db name;
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("bombardiloCrocodilo.db");
        return db;
}
