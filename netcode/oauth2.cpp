#include "oauth2.h"

OAuth2::OAuth2(QString dbPath,
               QString client_id,
               QString project_id,
               QString auth_uri,
               QString token_uri,
               QString auth_provider_x509_cert_url,
               QString client_secret,
               QStringList redirect_uris,
               QString scope,
               QObject *parent) : QObject(parent), Json2Sqlite()
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
    oauth2Settings_["api_scope"] = QJsonValue(scope);

    if(!oauth2Settings_["db_path"].toString().isEmpty())
        saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    else
        qDebug() << "Warning Oauth2 ctor: there is no database path, not saving settings";

    connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
        &QDesktopServices::openUrl);

    connect(google, &QOAuth2AuthorizationCodeFlow::granted,
        this, &OAuth2::saveOAuth2TokensToDB);

    build();
}


OAuth2::OAuth2(QString dbPath,
               QString googleJsonCredPath,
               QString scope,
               QObject *parent) : QObject(parent), Json2Sqlite()
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

    oauth2Settings_["api_scope"] = QJsonValue(scope);

    if(!oauth2Settings_["db_path"].toString().isEmpty())
        saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    else
        qDebug() << "Warning Oauth2 ctor: there is no database path, not saving settings";

    connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
        &QDesktopServices::openUrl);

    connect(google, &QOAuth2AuthorizationCodeFlow::granted,
        this, &OAuth2::saveOAuth2TokensToDB);

    build();
}

OAuth2::OAuth2(QString dbPath,
               QString scope,
               QObject *parent) : QObject(parent), Json2Sqlite()
{
    qDebug() << "oauth2 ctor overload1";

    if(!dbPath.isEmpty())
        oauth2Settings_["db_path"] = dbPath;

    if(!oauth2Settings_["db_path"].toString().isEmpty())
        oauth2Settings_ = loadSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    else
        qDebug() << "Warning Oauth2 ctor: there is no database path, not loading settings";

    oauth2Settings_["api_scope"] = QJsonValue(scope);

    if(!oauth2Settings_["db_path"].toString().isEmpty())
        saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    else
        qDebug() << "Warning Oauth2 ctor: there is no database path, not saving settings";

    connect(google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
        &QDesktopServices::openUrl);

    connect(google, &QOAuth2AuthorizationCodeFlow::granted,
        this, &OAuth2::saveOAuth2TokensToDB);

    build();
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

void OAuth2::setScope(const QString &scope)
{
    oauth2Settings_["api_scope"] = QJsonValue(scope);
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
    qDebug() << "Token from get after granted" << google->token();
    oauth2Settings_["token"] = google->token();
    oauth2Settings_["expiration_at"] = google->expirationAt().toString(Qt::ISODateWithMs);

    if(!google->refreshToken().isEmpty())
    {
        oauth2Settings_["refresh_token"] = google->refreshToken();
        google->setRefreshToken(oauth2Settings_["refresh_token"].toString());
    }

    saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    readyForRequests_ = true;
    return true;
}

bool OAuth2::build()
{
    bool success = true;

    google->setScope(oauth2Settings_["api_scope"].toString());

    const auto redirectUris = oauth2Settings_["redirect_uris"].toArray();
    const QUrl redirectUri(redirectUris[0].toString()); // Get the first URI
    const auto port = static_cast<quint16>(redirectUri.port()); // Get the port

    google->setAuthorizationUrl(oauth2Settings_["auth_uri"].toString());
    google->setClientIdentifier(oauth2Settings_["client_id"].toString());
    google->setAccessTokenUrl(oauth2Settings_["token_uri"].toString());
    google->setClientIdentifierSharedKey(oauth2Settings_["client_secret"].toString());

    google->setModifyParametersFunction\
        ([&](QAbstractOAuth::Stage stage, QVariantMap *parameters)\
         {
              if(stage == QAbstractOAuth::Stage::RequestingAuthorization)
              {
                 qDebug() << "Request Access";
                 parameters->insert("approval_prompt", "force");
                 parameters->insert("access_type", "offline");
                 qDebug() << "--------------------------------";
              }
              if(stage == QAbstractOAuth::Stage::RefreshingAccessToken)
              {
                  qDebug() << "Refresh Access";
                  parameters->clear();
                  parameters->insert("client_secret", oauth2Settings_["client_secret"].toString());
                  parameters->insert("refresh_token", oauth2Settings_["refresh_token"].toString());
                  parameters->insert("grant_type", "refresh_token");
                  parameters->insert("client_id", oauth2Settings_["client_id"].toString());
              }
          });

    auto replyHandler = new QOAuthHttpServerReplyHandler(port, this);
    google->setReplyHandler(replyHandler);

    if(oauth2Settings_["refresh_token"].toString().isEmpty())
    {
        google->grant();
    }
    else if(QDateTime::fromString(oauth2Settings_["expiration_at"].toString(), Qt::ISODateWithMs) < QDateTime::currentDateTime())
    {
        google->setRefreshToken(oauth2Settings_["refresh_token"].toString());
        google->setToken(oauth2Settings_["token"].toString());
        qDebug() << "from get after DB set" << google->token();
        google->refreshAccessToken();
    }
    else
    {
        google->setRefreshToken(oauth2Settings_["refresh_token"].toString());
        google->setToken(oauth2Settings_["token"].toString());
        qDebug() << "from get after DB set" << google->token();
        readyForRequests_ = true;
    }

    return success;
}

QByteArray OAuth2::get(const QString &address)
{
    while(!readyForRequests_)
    {
        qApp->processEvents();
        usleep(10);
    }
    QNetworkReply *response = google->get(QUrl(address));
    while(!response->isFinished())
    {
        qApp->processEvents();
        usleep(10);
    }
    QByteArray responseCopy = response->readAll();
    response->deleteLater();
    return responseCopy;
}

 bool OAuth2::tokens(const QByteArray &tokens)
 {
     qDebug() << tokens;
     return true;
 }
