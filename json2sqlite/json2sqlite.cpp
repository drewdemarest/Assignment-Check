#include "json2sqlite.h"

Json2Sqlite::Json2Sqlite(QString dbPath)
{
    qDebug() << "json2sqlite ctor";

    if(!doesDatabaseExist(dbPath))
        makeInitalDatabase(dbPath);
}

QJsonObject Json2Sqlite::loadSettings(QString dbPath, QJsonObject jsonSettings)
{
    QSqlDatabase database;
    database = database.addDatabase("QSQLITE", "settings");
    database.setDatabaseName(dbPath);

    if(!database.open())
    {
        qDebug() << "db failed to load in OAuth2 loadSettings"
                 << "All settings values are null!";

        return jsonSettings;
    }

    QSqlQuery query(database);
    QString queryString  = "SELECT * FROM settings WHERE key IN (";

    for(auto key: jsonSettings.keys()){
        queryString.append("'" + key + "', ");
    }
    queryString.remove((queryString.length() - 2), 2);
    queryString.append(")");

    query.prepare(queryString);

    if(query.exec())
    {
        qDebug() << "OAuth2::loadSettings query success.";

        while(query.next())
        {
            if(query.value("isJsonArray").toBool())
                jsonSettings[query.value("key").toString()] = QJsonDocument::fromJson(query.value("value").toString().toUtf8()).array();
            else
                jsonSettings[query.value("key").toString()] = query.value("value").toJsonValue();
        }
    }
    else
    {
        qDebug() << "OAuth2::loadSettings ERROR: " << query.lastError();
    }
    query.clear();

    database.close();
    database = QSqlDatabase();
    database.removeDatabase("settings");

    return jsonSettings;
}

bool Json2Sqlite::saveSettings(QString dbPath, QJsonObject jsonSettings)
{
    bool success = false;
    QString queryString = "INSERT or REPLACE into settings VALUES ";

    QSqlDatabase database;

    database = database.addDatabase("QSQLITE", "settings");
    database.setDatabaseName(dbPath);

    if(!database.open())
    {
        qDebug() << "db failed to load in OAuth2 loadSettings"
                 << "All settings values are null!";

        return false;
    }

    QSqlQuery query(database);

    for(auto key: jsonSettings.keys())
    {
        if(jsonSettings[key].isArray())
        {
            QJsonDocument arrayToString;
            arrayToString.setArray(jsonSettings[key].toArray());
            queryString.append("('" + key + "', '" + QString(arrayToString.toJson()) + "', '" + QString::number(true) + "'),");
        }
        else{
            queryString.append("('" + key + "', '" + jsonSettings[key].toString() + "', '" + QString::number(false) + "'),");
        }
    }
    queryString.remove(queryString.length() - 1, 1);

    query.prepare(queryString);
    if(query.exec())
    {
        qDebug() << "OAuth2::saveSettings query success.";
        success = true;
    }
    else
    {
        qDebug() << "OAuth2::saveSettings ERROR: " << query.lastError();
        success = false;
    }

    query.clear();
    database.close();
    database = QSqlDatabase();
    database.removeDatabase("settings");
    return success;
}

bool Json2Sqlite::doesDatabaseExist(QString dbPath)
{
    QFileInfo checkFile(dbPath);
    if(checkFile.exists() && checkFile.isFile())
        return true;
    else
        return false;
}

bool Json2Sqlite::makeInitalDatabase(QString dbPath)
{
    bool success;
    QSqlDatabase database;

    database = database.addDatabase("QSQLITE", "settings");
    database.setDatabaseName(dbPath);

    if(!database.open())
    {
        qDebug() << "db failed to load in OAuth2::makeInitalDatabase"
                 << "All settings values are null!";

        return false;
    }

    QSqlQuery query(database);
    query.prepare("CREATE TABLE settings(key text PRIMARY KEY, value text, isJsonArray integer)");
    success = query.exec();
    query.clear();

    database.close();
    database = QSqlDatabase();
    database.removeDatabase("settings");
    return success;
}
