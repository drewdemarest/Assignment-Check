#include "oauth2.h"

OAuth2::OAuth2(QObject *parent) : QObject(parent)
{
    loadSettings(dbPath_);
}

OAuth2::OAuth2(QString dbPath, QObject *parent) : QObject(parent)
{
        dbPath_ = dbPath;
        loadSettings(dbPath_);
}


bool OAuth2::setCredentialsFromJson(QString jsonCredPath)
{
    const QJsonObject credentials = makeJsonFromFile(jsonCredPath);

    clientId_           = QUrl(credentials["client_id"].toString());
    projectId_          = credentials["project_id"].toString();
    authUri_            = QUrl(credentials["auth_uri"].toString());
    tokenUri_           = QUrl(credentials["token_uri"].toString());
    clientSecret_       = credentials["client_secret"].toString();

    authProviderX509CertUrl_ \
                        = credentials["auth_provider_x509_cert_url"].toString();

    auto redirectUris   = credentials["redirect_uris"].toArray();
    redirectUri_        = QUrl(redirectUris[0].toString());
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
    QString sheetsScope;
    QString sheetsAddressBase;
    QString settingsPath_ = QApplication::applicationDirPath()+"/settings.db";
    qDebug() << settingsPath_;
    QSqlDatabase settings;
    settings = settings.addDatabase("QSQLITE", "settings");
    settings.setDatabaseName(settingsPath_);

    if(!settings.open())
    {
        qDebug() << "Settings failed to load in OAuth2 loadSettings"
                 << "All settings values are null!";

        return false;
    }

    QSqlQuery query(settings);

    query.prepare("SELECT value FROM masterRouteSettings WHERE \
                  variable = 'sheetsAddressBase'");
    query.exec();

    int i = 0;
    while(query.next())
    {
        qDebug() << query.value(i).toString() << i;
        i++;

        if(query.value(0).toString() == "sheetsScope")
            sheetsScope = query.value(1).toString();

        if(query.value(0).toString() == "sheetsAddressBase")
            sheetsAddressBase = query.value(2).toString();
        if(query.value(0).toString() == "sheetsScope")
            sheetsScope = query.value(1).toString();

        if(query.value(0).toString() == "sheetsAddressBase")
            sheetsAddressBase = query.value(2).toString();

    }

    qDebug() << sheetsAddressBase << sheetsScope;
    query.clear();


    settings.close();
    settings = QSqlDatabase();
    settings.removeDatabase("settings");
    return true;
}

bool OAuth2::saveSettings(QString dbPath)
{

}

QByteArray OAuth2::get()
{
    return QByteArray();
}
