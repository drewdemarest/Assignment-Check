#include "oauthnetconnect.h"

OAuthNetConnect::OAuthNetConnect(QObject *parent) :
    QObject(parent),
    oauthSettings(new QSettings(QApplication::applicationDirPath() + "/oauth.ini", QSettings::IniFormat))
{
    responseTimer->setInterval(timerDuration);
    responseTimer->setSingleShot(true);

    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::granted, this, &OAuthNetConnect::oauthGranted);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::error, this, &OAuthNetConnect::oauthFailedError);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::requestFailed, this, &OAuthNetConnect::oauthFailedRequestFailed);
    connect(responseTimer, &QTimer::timeout, this, &OAuthNetConnect::oauthFailedTimeout);
}

OAuthNetConnect::OAuthNetConnect(const QString &scope, const QString &address, const QString &credentialFilePath, QObject *parent) :
    QObject(parent),
    oauthSettings(new QSettings(QApplication::applicationDirPath() + "/oauth.ini", QSettings::IniFormat))
{
    responseTimer->setInterval(timerDuration);
    responseTimer->setSingleShot(true);

    //connections
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::granted, this, &OAuthNetConnect::oauthGranted);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::error, this, &OAuthNetConnect::oauthFailedError);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::requestFailed, this, &OAuthNetConnect::oauthFailedRequestFailed);
    connect(responseTimer, &QTimer::timeout, this, &OAuthNetConnect::oauthFailedTimeout);

    buildOAuth(scope, address, credentialFilePath);
}

OAuthNetConnect::~OAuthNetConnect()
{
    oauthSettings->deleteLater();
    responseTimer->deleteLater();
    oauth2NetworkAccess->deleteLater();

}

void OAuthNetConnect::buildOAuth(const QString &scope, const QString &address, const QString &credentialFilePath)
{
    if(!aborted)
    {
        while(waitingForOauth)
        {
            usleep(10);
            qDebug() << "Stuck waiting?";
            qApp->processEvents();
        }

        loadSettings();

        this->address = address;

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

        oauth2NetworkAccess->setModifyParametersFunction\
            ([&](QAbstractOAuth::Stage stage, QVariantMap *parameters)\
             {
                  if(stage == QAbstractOAuth::Stage::RequestingAuthorization)
                  {
                     parameters->insert("approvalPrompt", "force");
                     parameters->insert("accessType", "offfline");
                  }
              });

        if(tokenExpire < QDateTime(QDateTime::currentDateTime()))
        {
            auto replyHandler = new QOAuthHttpServerReplyHandler(port, this);
            oauth2NetworkAccess->setReplyHandler(replyHandler);
        }

        //check if the auth token from settings file is valid
        if(oauthToken.isNull())
        {
            waitingForOauth = true;
            oauth2NetworkAccess->grant();
        }
        else if(tokenExpire < QDateTime::currentDateTime())
        {
            waitingForOauth = true;
            oauth2NetworkAccess->grant();
        }
        else
        {
            oauthValid = true;
            waitingForOauth = false;
            oauth2NetworkAccess->setToken(oauthToken);
        }
    }
    else
        return;
}

QByteArray OAuthNetConnect::get()
{
    if(!aborted)
    {
        responseTimer->start();
        while(waitingForOauth)
        {
            usleep(10);
            qApp->processEvents();
            if(aborted){
                responseTimer->stop();
                oauthFailedTimeout();
                return QByteArray();
            }

        }
        responseTimer->stop();

        if(!oauthValid)
        {
            qDebug() << "OAuth2 failed in get";
            return QByteArray();
        }

        QNetworkReply *reply = oauth2NetworkAccess->get(QUrl(address));
        QByteArray replyCopy;

        while(!reply->isFinished())
        {
            usleep(10);
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
    else
        return QByteArray();
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
}

void OAuthNetConnect::saveSettings()
{
    oauthSettings->setValue("oauth2/token", oauth2NetworkAccess->token());
    oauthSettings->setValue("oauth2/tokenExpire", QVariant(tokenExpire));
}

void OAuthNetConnect::debugReply()
{
    //authGranted = true;
    qDebug() << int(oauth2NetworkAccess->status());
    qDebug() << QString(oauth2NetworkAccess->token());
    qDebug() << oauth2NetworkAccess->authorizationUrl();
    QNetworkReply *reply = oauth2NetworkAccess->get(QUrl(address));

    responseTimer->start();
    while(waitingForOauth)
    {
        usleep(10);
        qApp->processEvents();

    }
    responseTimer->stop();

    if(!oauthValid)
    {
        qDebug() << "OAuth2 failed in debug.";
        return;
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
    oauthValid = true;
    oauthToken = oauth2NetworkAccess->token();

    tokenExpire = oauth2NetworkAccess->expirationAt();
    saveSettings();
    oauthSettings->sync();
    emit succeeded();
}

void OAuthNetConnect::oauthFailedTimeout()
{
    auto rh = oauth2NetworkAccess->replyHandler();
    rh->thread()->quit();
    delete rh;
    rh = Q_NULLPTR;

    delete oauth2NetworkAccess;
    oauth2NetworkAccess = Q_NULLPTR;

    oauth2NetworkAccess = new QOAuth2AuthorizationCodeFlow(this);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::granted, this, &OAuthNetConnect::oauthGranted);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::error, this, &OAuthNetConnect::oauthFailedError);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::requestFailed, this, &OAuthNetConnect::oauthFailedRequestFailed);
    connect(responseTimer, &QTimer::timeout, this, &OAuthNetConnect::oauthFailedTimeout);

    waitingForOauth = false;
    oauthValid = false;
    qDebug() << "OAuth2 Failed due to time out.";
    emit failed();
}

void OAuthNetConnect::oauthFailedError(const QString &error, const QString &errorDescription, const QUrl &uri)
{
    auto rh = oauth2NetworkAccess->replyHandler();
    rh->thread()->quit();
    delete rh;
    rh = Q_NULLPTR;

    delete oauth2NetworkAccess;
    oauth2NetworkAccess = Q_NULLPTR;

    oauth2NetworkAccess = new QOAuth2AuthorizationCodeFlow(this);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::granted, this, &OAuthNetConnect::oauthGranted);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::error, this, &OAuthNetConnect::oauthFailedError);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::requestFailed, this, &OAuthNetConnect::oauthFailedRequestFailed);
    connect(responseTimer, &QTimer::timeout, this, &OAuthNetConnect::oauthFailedTimeout);

    waitingForOauth = false;
    oauthValid = false;
    qDebug() << "OAuth2 Failed with error" << error << errorDescription << uri.toString();
    emit failed();
}

void OAuthNetConnect::oauthFailedRequestFailed(const QAbstractOAuth::Error error)
{
    auto rh = oauth2NetworkAccess->replyHandler();
    rh->thread()->quit();
    delete rh;
    rh = Q_NULLPTR;

    delete oauth2NetworkAccess;
    oauth2NetworkAccess = Q_NULLPTR;

    oauth2NetworkAccess = new QOAuth2AuthorizationCodeFlow(this);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::granted, this, &OAuthNetConnect::oauthGranted);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::error, this, &OAuthNetConnect::oauthFailedError);
    connect(oauth2NetworkAccess, &QOAuth2AuthorizationCodeFlow::requestFailed, this, &OAuthNetConnect::oauthFailedRequestFailed);
    connect(responseTimer, &QTimer::timeout, this, &OAuthNetConnect::oauthFailedTimeout);

    waitingForOauth = false;
    oauthValid = false;
    switch (error)
    {
    case QAbstractOAuth::Error::NoError:
        qDebug() << "No error has ocurred.";
        break;

    case QAbstractOAuth::Error::NetworkError:
        qDebug() << "Failed to connect to the server.";
        break;

    case QAbstractOAuth::Error::ServerError:
        qDebug() << "The server answered the request"
                    " with an error.";
        break;

    case QAbstractOAuth::Error::OAuthTokenNotFoundError:
        qDebug() << "The server's response to a token"
                    " request provided no token identifier.";
        break;

    case QAbstractOAuth::Error::OAuthTokenSecretNotFoundError:
        qDebug() << "	The server's response to a token"
                    " request provided no token secret.";
        break;

    case QAbstractOAuth::Error::OAuthCallbackNotVerified:
        qDebug() << "	The authorization server has not"
                    " verified the supplied callback URI in"
                    " the request. This usually happens"
                    " when the provided callback does"
                    " not match with the callback"
                    " supplied during client registration.";
        break;

    }
    emit failed();
}

void OAuthNetConnect::abort(bool abortStatus)
{
    qDebug() << "aborted?" << abortStatus;
    aborted = abortStatus;
}

void OAuthNetConnect::setResponseWaitTime(int timerDuration)
{
    this->timerDuration = timerDuration;
}
