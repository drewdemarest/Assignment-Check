#include "oauth2.h"

OAuth2::OAuth2(QObject *parent) : QObject(parent)
{
    qDebug() << "SUMMONED!";
    if(!doesDatabaseExist(oauth2Settings_["db_path"].toString()))
        makeInitalDatabase(oauth2Settings_["db_path"].toString());

    //loadSettings(oauth2Settings_["db_path"].toString());
    //qDebug() << oauth2Settings_["auth_provider_x509_cert_url"].toString();

    //setCredentialsFromJsonFile(QApplication::applicationDirPath() + "/client.json");
    //saveSettings(oauth2Settings_["db_path"].toString());
    loadSettings(oauth2Settings_["db_path"].toString());
    qDebug() << oauth2Settings_;
}

OAuth2::OAuth2(QString dbPath, QObject *parent) : QObject(parent)
{

    qDebug() << "SUMMONED! AND OVERLOADED, PHORRRWAR";

    oauth2Settings_["db_path"] = dbPath;
    loadSettings(oauth2Settings_["db_path"].toString());
    if(!doesDatabaseExist(oauth2Settings_["db_path"].toString()))
        makeInitalDatabase(oauth2Settings_["db_path"].toString());

    loadSettings(oauth2Settings_["db_path"].toString());
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

    if(!saveSettings(oauth2Settings_["db_path"].toString()))
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
                oauth2Settings_["redirect_uris"] = QJsonDocument::fromJson(query.value("value").toString().toUtf8()).array();
            else
                oauth2Settings_[query.value("key").toString()] = query.value("value").toJsonValue();
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
    bool success = false;
    QString queryString = "INSERT or REPLACE into oauth2Credentials VALUES ";

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
        if(oauth2Settings_[key].isArray())
        {
            QJsonDocument arrayToString;
            arrayToString.setArray(oauth2Settings_[key].toArray());
            queryString.append("('" + key + "', '" + QString(arrayToString.toJson()) + "', '" + QString::number(true) + "'),");
        }
        else{
            queryString.append("('" + key + "', '" + oauth2Settings_[key].toString() + "', '" + QString::number(false) + "'),");
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
