#include "oauthnetconnect.h"

OAuthNetConnect::OAuthNetConnect(QObject *parent) :
    QObject(parent),
    oauthSettings(new QSettings(QApplication::applicationDirPath() + "/oauth.ini", QSettings::IniFormat))
{
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::granted, this, &OAuthNetConnect::oauthGranted);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::requestFailed, this, &OAuthNetConnect::oauthFailed);
}

OAuthNetConnect::OAuthNetConnect(const QString &scope, const QString &address, const QString &credentialFilePath, QObject *parent) :
    QObject(parent),
    oauthSettings(new QSettings(QApplication::applicationDirPath() + "/oauth.ini", QSettings::IniFormat))
{
    //connections
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::granted, this, &OAuthNetConnect::oauthGranted);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::requestFailed, this, &OAuthNetConnect::oauthFailed);
    buildOAuth(scope, address, credentialFilePath);
}

OAuthNetConnect::~OAuthNetConnect()
{
    oauthSettings->deleteLater();
}

void OAuthNetConnect::buildOAuth(const QString &scope, const QString &address, const QString &credentialFilePath)
{
    loadSettings();
    qDebug() << "build: token expires at" << tokenExpire.toString();

    while(waitingForOauth)
    {
        usleep(1000);
        qDebug() << "beep";
        qApp->processEvents();
    }

    this->address = address;
    qDebug() << oauth2NetworkAccess->dynamicPropertyNames();
    //temporary code-- for debuggin server replies
    //connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::granted, this, &NetConnect::debugReply);

    //google->setScope("https://www.googleapis.com/auth/spreadsheets.readonly");
    oauth2NetworkAccess->setScope(scope);

    const QJsonObject object = readJsonCredentials(credentialFilePath);

    const auto settingsObject = object["web"].toObject();

    const QUrl authUri(settingsObject["auth_uri"].toString());
    const auto clientId = settingsObject["client_id"].toString();
    const QUrl tokenUri(settingsObject["token_uri"].toString());
    const auto clientSecret(settingsObject["client_secret"].toString());
    const auto redirectUris = settingsObject["redirect_uris"].toArray();
    const QUrl redirectUri(redirectUris[0].toString()); // Get the first URI
    const auto port = static_cast<quint16>(redirectUri.port()); // Get the port

    oauth2NetworkAccess->setAuthorizationUrl(authUri);
    oauth2NetworkAccess->setClientIdentifier(clientId);
    oauth2NetworkAccess->setAccessTokenUrl(tokenUri);
    oauth2NetworkAccess->setClientIdentifierSharedKey(clientSecret);

    if(tokenExpire < QDateTime(QDateTime::currentDateTime()))
    {
        //Seems like reply handler always exists within QOAuth2AuthorizationCodeFlow...
        //So I delete whatever is in there and then allocate on the stack.
        //oauth2NetworkAccess->replyHandler()->deleteLater();

        auto replyHandler = new QOAuthHttpServerReplyHandler(port, this);
        oauth2NetworkAccess->setReplyHandler(replyHandler);
    }

    //check if the auth token from settings file is valid
    qDebug() << "oauthTok null?"  << oauthToken.isNull() << "empty?" << oauthToken.isEmpty();
    if(oauthToken.isNull())
    {
        qDebug() << "Here 1";
        waitingForOauth = true;
        oauth2NetworkAccess->grant();
    }
    else if(tokenExpire < QDateTime::currentDateTime())
    {
        qDebug() << "Here 2";
        waitingForOauth = true;
        oauth2NetworkAccess->grant();
    }
    else
    {
        qDebug() << "Here 3";
        waitingForOauth = false;
        oauth2NetworkAccess->setToken(oauthToken);
    }
    //oauth2NetworkAccess->requestFailed();
    //debugReply();

    //oauth2NetworkAccess->grant();
    //oauth test end
}



QJsonObject OAuthNetConnect::readJsonCredentials(const QString &credentialFilePath)
{
    QByteArray credentials;

    QFile jsonCredentialFile(credentialFilePath);
    if(!jsonCredentialFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return QJsonObject();

    while(!jsonCredentialFile.atEnd())
    {
        credentials.append(jsonCredentialFile.readLine());
    }

    return QJsonObject(QJsonDocument::fromJson(credentials).object());
}

void OAuthNetConnect::loadSettings()
{
    oauthToken = oauthSettings->value("oauth2/token").toString();
    tokenExpire = oauthSettings->value("oauth2/tokenExpire").toDateTime();
    qDebug() << "null?" << tokenExpire.isNull() << "valid" << tokenExpire.isValid();
}

void OAuthNetConnect::saveSettings()
{
    qDebug() << "save: token expires at" << tokenExpire.toString();
    oauthSettings->setValue("oauth2/token", oauth2NetworkAccess->token());
    oauthSettings->setValue("oauth2/tokenExpire", QVariant(tokenExpire));
}

QByteArray OAuthNetConnect::get()
{
    qDebug() << "before oauth" << waitingForOauth;

    while(waitingForOauth)
    {
        usleep(1000);
        qApp->processEvents();
    }

    qDebug() << "after oauth" << waitingForOauth;

    QNetworkReply *reply = oauth2NetworkAccess->get(QUrl(address));
    QByteArray replyCopy;

    while(!reply->isFinished())
    {
        usleep(1000);
        qApp->processEvents();
    }

    if(reply->errorString() == QNetworkReply::AuthenticationRequiredError)
    {
        qDebug() << reply->errorString();
        oauth2NetworkAccess->grant();
    }
    else
        replyCopy = reply->readAll();

    reply->deleteLater();
    return replyCopy;
}

void OAuthNetConnect::debugReply()
{
    //authGranted = true;
    qDebug() << int(oauth2NetworkAccess->status());
    qDebug() << QString(oauth2NetworkAccess->token());
    qDebug() << oauth2NetworkAccess->authorizationUrl();
    QNetworkReply *reply = oauth2NetworkAccess->get(QUrl(address));

    while(waitingForOauth)
    {
        usleep(1000);
        qApp->processEvents();
    }
    if(reply->errorString() == QNetworkReply::AuthenticationRequiredError)
    {
        qDebug() << reply->errorString();
        oauth2NetworkAccess->grant();
    }
    if(reply->errorString() != QNetworkReply::NoError)
    {
       qDebug() << reply->errorString();
    }
    else
        qDebug() << reply->readAll();

    reply->deleteLater();
}

void OAuthNetConnect::oauthGranted()
{
    waitingForOauth = false;
    oauthToken = oauth2NetworkAccess->token();

    tokenExpire = oauth2NetworkAccess->expirationAt();
    saveSettings();
    oauthSettings->sync();
}

void OAuthNetConnect::oauthFailed()
{
    waitingForOauth = false;
    qDebug() << "Failed.";
}

bool OAuthNetConnect::isWaitingForOauth()
{
    return waitingForOauth;
}

