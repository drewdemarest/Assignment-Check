#include "oauth2.h"

OAuth2::OAuth2(QObject *parent) : QObject(parent)
{
    qDebug() << "SUMMONED!";
    if(!doesDatabaseExist(dbPath_.toString()))
        makeInitalDatabase(dbPath_.toString());

    setCredentialsFromJsonFile(QApplication::applicationDirPath() + "/client.json");
    saveSettings(dbPath_.toString());
    loadSettings(dbPath_.toString());
}

OAuth2::OAuth2(QString dbPath, QObject *parent) : QObject(parent)
{
    qDebug() << "SUMMONED! AND OVERLOADED, PHORRRWAR";

    dbPath_ = dbPath;

    if(!doesDatabaseExist(dbPath_.toString()))
        makeInitalDatabase(dbPath_.toString());

    loadSettings(dbPath_.toString());
}


bool OAuth2::setCredentialsFromJsonFile(QString jsonCredPath)
{
    const QJsonObject credentials = makeJsonFromFile(jsonCredPath);
    const QJsonObject credentialWebSection = credentials["web"].toObject();

    for(auto jsonKey: credentialWebSection.keys())
    {
        if(oauth2Settings_.contains(jsonKey))
        {
            oauth2Settings_[jsonKey] = credentialWebSection[jsonKey];
        }
    }

    if(!saveSettings(dbPath_.toString()))
    {
        qDebug() << "Failed to save json credentials to db" \
                 << "in OAuth2::setCredentialsFromJson";
        return false;
    }

    return true;
}

QJsonObject OAuth2::makeJsonFromFile(QString jsonCredentialPath)
{
    QByteArray credentials;
    QFile jsonCredentialFile(jsonCredentialPath);

    if(!jsonCredentialFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open json credential file in\
                    OAuth2::setOAuth2CredWithJson";
                    return QJsonObject();
    }

    while(!jsonCredentialFile.atEnd())
    {
        credentials.append(jsonCredentialFile.readLine());
    }

    return QJsonObject(QJsonDocument::fromJson(credentials).object());
}

bool OAuth2::loadSettings(QString dbPath)
{
    QSqlDatabase oauth2Settings;
    oauth2Settings = oauth2Settings.addDatabase("QSQLITE", "settings");
    oauth2Settings.setDatabaseName(dbPath);

    if(!oauth2Settings.open())
    {
        qDebug() << "db failed to load in OAuth2 loadSettings"
                 << "All settings values are null!";

        return false;
    }

    QSqlQuery query(oauth2Settings);
    QString queryString  = "SELECT * FROM oauth2Credentials WHERE key IN (";

    for(auto key: oauth2Settings_.keys()){
        queryString.append("'" + key + "'" + QString(", "));
    }

    queryString.remove((queryString.length() - 2), 2);
    queryString.append(")");

    qDebug() << queryString;

    //query.prepare("SELECT value FROM oauth2Credentials WHERE key = (:key)");
    //query.bindValue(":key", key);
    query.prepare(queryString);

    if(query.exec())
    {
        qDebug() << "OAuth2::loadSettings query success.";

        while(query.next())
        {
            qDebug() << "--------------------------------";
            qDebug() << query.value("key").toString();
            qDebug() << query.value("value").toString();
            qDebug() << query.value("isJsonArray").toString();
            qDebug() << "--------------------------------";
        }
    }
    else
    {
        qDebug() << "OAuth2::loadSettings ERROR: " << query.lastError();
    }
    query.clear();

    oauth2Settings.close();
    oauth2Settings = QSqlDatabase();
    oauth2Settings.removeDatabase("settings");
    return true;
}

bool OAuth2::saveSettings(QString dbPath)
{
    bool success;
    bool isJsonArray = false;
    QString valueTuple;

    QSqlDatabase oauth2Settings;

    oauth2Settings = oauth2Settings.addDatabase("QSQLITE", "settings");
    oauth2Settings.setDatabaseName(dbPath);

    if(!oauth2Settings.open())
    {
        qDebug() << "db failed to load in OAuth2 loadSettings"
                 << "All settings values are null!";

        return false;
    }

    QSqlQuery query(oauth2Settings);

    for(auto key: oauth2Settings_.keys())
    {
        if(oauth2Settings_[key].toJsonValue().isArray())
        {
            QJsonDocument arrayToString;
            arrayToString.setArray(oauth2Settings_[key].toJsonArray());
            valueTuple.append("('" + key + "', '" + QString(arrayToString.toJson()) + "', '" + QString::number(true) + "'),");
        }
        else{
            valueTuple.append("('" + key + "', '" + oauth2Settings_[key].toString() + "', '" + QString::number(false) + "'),");
        }
    }
    valueTuple.remove(valueTuple.length() - 1, 1);

    qDebug() << valueTuple;

    query.prepare("INSERT or REPLACE into oauth2Credentials VALUES " + valueTuple);
    if(query.exec())
    {
        qDebug() << "OAuth2::saveSettings query success.";
    }
    else
    {
        qDebug() << "OAuth2::saveSettings ERROR: " << query.lastError();
    }

    query.clear();

    oauth2Settings.close();
    oauth2Settings = QSqlDatabase();
    oauth2Settings.removeDatabase("settings");
    return success;
}

bool OAuth2::doesDatabaseExist(QString dbPath)
{
    QFileInfo checkFile(dbPath);
    if(checkFile.exists() && checkFile.isFile())
        return true;
    else
        return false;
}

bool OAuth2::makeInitalDatabase(QString dbPath)
{
    bool success;
    QSqlDatabase oauth2Settings;

    oauth2Settings = oauth2Settings.addDatabase("QSQLITE", "settings");
    oauth2Settings.setDatabaseName(dbPath);

    if(!oauth2Settings.open())
    {
        qDebug() << "db failed to load in OAuth2::makeInitalDatabase"
                 << "All settings values are null!";

        return false;
    }

    QSqlQuery query(oauth2Settings);
    query.prepare("CREATE TABLE oauth2Credentials(key text PRIMARY KEY, value text, isJsonArray integer)");
    success = query.exec();
    query.clear();

    oauth2Settings.close();
    oauth2Settings = QSqlDatabase();
    oauth2Settings.removeDatabase("settings");
    return success;
}

QByteArray OAuth2::get()
{
    return QByteArray();
}
