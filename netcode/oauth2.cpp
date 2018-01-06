#include "oauth2.h"

OAuth2::OAuth2(QObject *parent) : QObject(parent), Json2Sqlite()
{
    qDebug() << "SUMMONED!";

    setCredentialsFromJsonFile(QApplication::applicationDirPath() + "/client.json");
    loadSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    qDebug() << oauth2Settings_;
}

OAuth2::OAuth2(QString dbPath, QObject *parent) : QObject(parent), Json2Sqlite()
{
    qDebug() << "SUMMONED! AND OVERLOADED, PHORRRWAR";

    oauth2Settings_["db_path"] = dbPath;
    loadSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
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

    if(!saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_))
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



QByteArray OAuth2::get()
{
    return QByteArray();
}
