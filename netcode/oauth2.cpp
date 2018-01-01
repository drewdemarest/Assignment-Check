#include "oauth2.h"

OAuth2::OAuth2(QObject *parent) : QObject(parent)
{
    qDebug() << "SUMMONED!";
    if(!doesDatabaseExist(dbPath_))
        makeInitalDatabase(dbPath_);

    setCredentialsFromJsonFile(QApplication::applicationDirPath() + "/client.json");
    saveSettings(dbPath_);
    loadSettings(dbPath_);
}

OAuth2::OAuth2(QString dbPath, QObject *parent) : QObject(parent)
{
    qDebug() << "SUMMONED! AND OVERLOADED, PHORRRWAR";
    dbPath_ = dbPath;
    if(!doesDatabaseExist(dbPath_))
        makeInitalDatabase(dbPath_);

    loadSettings(dbPath_);
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

//    clientId_           = QUrl(credentialWebSection["client_id"].toString());
//    projectId_          = credentialWebSection["project_id"].toString();
//    authUri_            = QUrl(credentialWebSection["auth_uri"].toString());
//    tokenUri_           = QUrl(credentialWebSection["token_uri"].toString());
//    clientSecret_       = credentialWebSection["client_secret"].toString();

//    authProviderX509CertUrl_ = credentialWebSection["auth_provider_x509_cert_url"].toString();

//    auto redirectUris   = credentialWebSection["redirect_uris"].toArray();

//    qDebug() << redirectUris.size();
//    redirectUri_        = redirectUris[0].toString();
//    port_               = static_cast<quint16>(redirectUri_.port());

    qDebug() << "Dingo dingo!" << oauth2Settings_;

    if(!saveSettings(dbPath_))
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


    for(auto key: oauth2Settings_.keys()){
        query.prepare("SELECT value FROM oauth2Credentials WHERE key = (:key)");
        query.bindValue(":key", key);

        if(query.exec())
        {
            qDebug() << key << "OAuth2::loadSettings query success.";

            while(query.next())
            {
                qDebug() << query.value("value").toString();
            }
        }
        else
        {
            qDebug() << "OAuth2::loadSettings ERROR: " << query.lastError();
        }
        query.clear();
    }

    qDebug() << queryRequestUrl_ << apiScope_ << "???";

    oauth2Settings.close();
    oauth2Settings = QSqlDatabase();
    oauth2Settings.removeDatabase("settings");
    return true;
}

bool OAuth2::saveSettings(QString dbPath)
{
    bool success;
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

    for(auto t: oauth2Settings_.keys())
    {
        if(oauth2Settings_[t].toJsonValue().isArray())
        {
            QJsonDocument arrayToString;
            arrayToString.setArray(oauth2Settings_[t].toJsonArray());

            query.prepare("INSERT or REPLACE into oauth2Credentials (key, value) VALUES (:key, :value)");
            query.bindValue(":key", t);
            query.bindValue(":value", QString(arrayToString.toJson()));

            if(query.exec())
            {
                qDebug() << t << "OAuth2::saveSettings query success.";
            }
            else
            {
                qDebug() << "OAuth2::saveSettings ERROR: " << query.lastError();
            }

            query.clear();
        }
        else{
            query.prepare("INSERT or REPLACE into oauth2Credentials (key, value) VALUES (:key, :value)");
            //oauth2Settings_[t] = "derpaderpakgjaslkfjalsk";
            query.bindValue(":key", t);
            query.bindValue(":value", oauth2Settings_[t]);

            if(query.exec())
            {
                qDebug() << t << "OAuth2::saveSettings query success.";
            }
            else
            {
                qDebug() << "OAuth2::saveSettings ERROR: " << query.lastError();
            }

            query.clear();
        }
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
    query.prepare("CREATE TABLE oauth2Credentials(key text primary key, value text)");
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
