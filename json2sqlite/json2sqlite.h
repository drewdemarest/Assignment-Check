#ifndef JSON2SQLITE_H
#define JSON2SQLITE_H

#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QApplication>
#include <QSqlError>

class Json2Sqlite
{
public:
    Json2Sqlite(QString dbPath);
    bool saveSettings(QString dbPath, QJsonObject jsonSettings);
    QJsonObject loadSettings(QString dbPath, QJsonObject jsonSettings);
    bool doesDatabaseExist(QString dbPath);
private:
    bool makeInitalDatabase(QString dbPath);
};

#endif // JSON2SQLITE_H
