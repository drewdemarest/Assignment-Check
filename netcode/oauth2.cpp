#include "oauth2.h"

OAuth2::OAuth2(QString dbPath,
               QString client_id,
               QString project_id,
               QString auth_uri,
               QString token_uri,
               QString auth_provider_x509_cert_url,
               QString client_secret,
               QStringList redirect_uris,
               QObject *parent,
               QString scope) : QObject(parent), Json2Sqlite()
{
    qDebug() << "oauth2 ctor overload 0";

    if(!dbPath.isEmpty())
        oauth2Settings_["db_path"] = dbPath;

    if(!oauth2Settings_["db_path"].toString().isEmpty())
        oauth2Settings_ = loadSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    else
        qDebug() << "Warning Oauth2 ctor: there is no database path, not loading settings";

    oauth2Settings_["client_id"] = QJsonValue(client_id);
    oauth2Settings_["project_id"] = QJsonValue(project_id);
    oauth2Settings_["auth_uri"] = QJsonValue(auth_uri);
    oauth2Settings_["token_uri"] = QJsonValue(token_uri);
    oauth2Settings_["auth_provider_x509_cert_url"] = QJsonValue(auth_provider_x509_cert_url);
    oauth2Settings_["client_secret"] = QJsonValue(client_secret);
    oauth2Settings_["redirect_uris"] = QJsonArray::fromStringList(redirect_uris);
    oauth2Settings_["scope"] = QJsonValue(scope);

    if(!oauth2Settings_["db_path"].toString().isEmpty())
        saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    else
        qDebug() << "Warning Oauth2 ctor: there is no database path, not saving settings";

    connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
        &QDesktopServices::openUrl);

    connect(google, &QOAuth2AuthorizationCodeFlow::granted,
        this, &OAuth2::saveOAuth2TokensToDB);
}


OAuth2::OAuth2(QString dbPath,
               QString googleJsonCredPath,
               QObject *parent,
               QString scope) : QObject(parent), Json2Sqlite()
{
    qDebug() << "oauth2 ctor overload1";

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

    oauth2Settings_["scope"] = QJsonValue(scope);

    if(!oauth2Settings_["db_path"].toString().isEmpty())
        saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    else
        qDebug() << "Warning Oauth2 ctor: there is no database path, not saving settings";

    connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
        &QDesktopServices::openUrl);

    connect(google, &QOAuth2AuthorizationCodeFlow::granted,
        this, &OAuth2::saveOAuth2TokensToDB);
}

OAuth2::OAuth2(QString dbPath,
               QObject *parent,
               QString scope) : QObject(parent), Json2Sqlite()
{
    qDebug() << "oauth2 ctor overload1";

    if(!dbPath.isEmpty())
        oauth2Settings_["db_path"] = dbPath;

    if(!oauth2Settings_["db_path"].toString().isEmpty())
        oauth2Settings_ = loadSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    else
        qDebug() << "Warning Oauth2 ctor: there is no database path, not loading settings";

    oauth2Settings_["scope"] = QJsonValue(scope);

    if(!oauth2Settings_["db_path"].toString().isEmpty())
        saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    else
        qDebug() << "Warning Oauth2 ctor: there is no database path, not saving settings";

    connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
        &QDesktopServices::openUrl);

    connect(google, &QOAuth2AuthorizationCodeFlow::granted,
        this, &OAuth2::saveOAuth2TokensToDB);
}

bool OAuth2::setCredentialsFromJsonFile(const QString &jsonCredPath)
{
    bool success = false;

    const QJsonObject credentials = makeJsonFromFile(jsonCredPath);

    if(credentials.isEmpty())
        return success;

    else
        success = true;

    const QJsonObject credentialWebSection = credentials["web"].toObject();

    for(auto jsonKey: credentialWebSection.keys())
    {
        if(oauth2Settings_.contains(jsonKey))
        {
            oauth2Settings_[jsonKey] = credentialWebSection[jsonKey];
        }
    }

    return success;
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

bool OAuth2::saveOAuth2TokensToDB()
{
    qDebug() << google->refreshToken();
    google->refreshAccessToken();
    oauth2Settings_["token"] = google->token();
    oauth2Settings_["expiration_at"] = google->expirationAt().toString();
    oauth2Settings_["refresh_token"] = google->refreshToken();
    saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    return true;
}



QByteArray OAuth2::get()
{

    google->setScope("https://www.googleapis.com/auth/spreadsheets.readonly");
    const QUrl authUri(oauth2Settings_["auth_uri"].toString());
    const auto clientId = oauth2Settings_["client_id"].toString();
    const QUrl tokenUri(oauth2Settings_["token_uri"].toString());
    const auto clientSecret(oauth2Settings_["client_secret"].toString());
    const auto redirectUris = oauth2Settings_["redirect_uris"].toArray();
    const QUrl redirectUri(redirectUris[0].toString()); // Get the first URI
    const auto port = static_cast<quint16>(redirectUri.port()); // Get the port

    google->setAuthorizationUrl(authUri);
    google->setClientIdentifier(clientId);
    google->setAccessTokenUrl(tokenUri);
    google->setClientIdentifierSharedKey(clientSecret);

    auto replyHandler = new QOAuthHttpServerReplyHandler(port, this);
    google->setReplyHandler(replyHandler);

    google->grant();


    auto reply = google->get(QUrl("https://www.googleapis.com/plus/v1/people/me"));
    qDebug() << reply;
    return QByteArray();
}
