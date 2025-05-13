#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DatabaseManager
{
public:
    static void initialize();
    static QString get(const QString &key);
    static bool set(const QString &key, const QString &value);

private:
    static QSqlDatabase getDatabase();
};

#endif // DATABASEMANAGER_H
