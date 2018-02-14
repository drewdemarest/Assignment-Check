#include "oauth2.h"

//OAuth2::OAuth2(QObject *parent) : QObject(parent), Json2Sqlite()
//{
//    qDebug() << "oauth2 ctor overload0";

//    oauth2Settings_ = loadSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
//    qDebug() << oauth2Settings_;

//    //removed because as the default constructor, this should just assume that the settings db is default and exists.

//    //setCredentialsFromJsonFile(oauth2Settings_["json_credential_path"].toString());
//    //saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);

//    /*
//    //test code to make big old array from file
//    QByteArray testQBA;
//    QFile testArrFile(QString(QApplication::applicationDirPath() + "/tst.txt"));

//    if(!testArrFile.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        qDebug() << "Failed to open test file";
//    }

//    while(!testArrFile.atEnd())
//    {
//        testQBA.append(testArrFile.readLine());
//    }
//    //testQBA.replace("'", QString(QStringLiteral("\\\'")).toUtf8());
//    //qDebug() << testQBA;
//    QJsonArray testArr = QJsonDocument::fromJson(testQBA).array();
//    qDebug() << testArr.size();
//    testArrFile.close();

//    saveArray(QString(QApplication::applicationDirPath() + "/test.db"), "dingo", "id", testArr);
////end test code
//*/
//}

//OAuth2::OAuth2(QString dbPath, QObject *parent) : QObject(parent), Json2Sqlite()
//{
//    qDebug() << "oauth2 ctor overload1";

//    oauth2Settings_["db_path"] = dbPath;
//    oauth2Settings_ = loadSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
//}

//OAuth2::OAuth2(QString dbPath, QString googleJsonCredPath, QObject *parent) : QObject(parent), Json2Sqlite()
//{

//}


OAuth2::OAuth2(QString dbPath,
               QObject *parent,
               QString client_id,
               QString project_id,
               QString auth_uri,
               QString token_uri,
               QString auth_provider_x509_cert_url,
               QString client_secret,
               QStringList redirect_uris,
               QString googleJsonCredPath) : QObject(parent), Json2Sqlite()
{
    qDebug() << "oauth2 ctor overload2";

    if(!dbPath.isEmpty())
        oauth2Settings_["db_path"] = dbPath;

    if(!oauth2Settings_["db_path"].toString().isEmpty())
        oauth2Settings_ = loadSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    else
        qDebug() << "Warning Oauth2 ctor: there is no database path, not loading settings";

    if(!googleJsonCredPath.isEmpty())
    {
        oauth2Settings_["json_credential_path"] = QJsonValue(googleJsonCredPath);
        setCredentialsFromJsonFile(oauth2Settings_["json_credential_path"].toString());
    }

    if(!client_id.isEmpty())
        oauth2Settings_["client_id"] = QJsonValue(client_id);

    if(!project_id.isEmpty())
        oauth2Settings_["project_id"] = QJsonValue(project_id);

    if(!auth_uri.isEmpty())
        oauth2Settings_["auth_uri"] = QJsonValue(auth_uri);

    if(!token_uri.isEmpty())
       oauth2Settings_["token_uri"] = QJsonValue(token_uri);

    if(!auth_provider_x509_cert_url.isEmpty())
        oauth2Settings_["auth_provider_x509_cert_url"] = QJsonValue(auth_provider_x509_cert_url);

    if(!client_secret.isEmpty())
        oauth2Settings_["client_secret"] = QJsonValue(client_secret);

    if(!redirect_uris.isEmpty())
        oauth2Settings_["client_id"] = QJsonArray::fromStringList(redirect_uris);

    if(!oauth2Settings_["db_path"].toString().isEmpty())
        saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    else
        qDebug() << "Warning Oauth2 ctor: there is no database path, not saving settings";


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

    jsonCredentialFile.close();
    return QJsonObject(QJsonDocument::fromJson(credentials).object());
}



QByteArray OAuth2::get()
{
    auto google = new QOAuth2AuthorizationCodeFlow;
    google->setScope("https://www.googleapis.com/auth/spreadsheets.readonly");
    return QByteArray();
}
