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

    clientId_           = QUrl(credentialWebSection["client_id"].toString());
    projectId_          = credentialWebSection["project_id"].toString();
    authUri_            = QUrl(credentialWebSection["auth_uri"].toString());
    tokenUri_           = QUrl(credentialWebSection["token_uri"].toString());
    clientSecret_       = credentialWebSection["client_secret"].toString();

    authProviderX509CertUrl_ = credentialWebSection["auth_provider_x509_cert_url"].toString();

    auto redirectUris   = credentialWebSection["redirect_uris"].toArray();

    qDebug() << redirectUris.size();
    redirectUri_        = redirectUris[0].toString();
    port_               = static_cast<quint16>(redirectUri_.port());

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

    query.prepare("SELECT value FROM masterRouteSettings WHERE"
                  "variable = 'sheetsAddressBase'");

    query.exec();

    int i = 0; //???

    while(query.next())
    {
        qDebug() << query.value(i).toString() << i;
        i++;

        if(query.value(0).toString() == "sheetsScope")
            apiScope_ = query.value(1).toString();

        if(query.value(0).toString() == "sheetsAddressBase")
            queryRequestUrl_ = query.value(2).toString();
    }

    qDebug() << queryRequestUrl_ << apiScope_ << "???";
    query.clear();


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
        oauth2Settings_[t] = "derpaderpakgjaslkfjalsk";
        query.prepare("INSERT or REPLACE into oauth2Credentials (key, value) VALUES (:key, :value)");
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
