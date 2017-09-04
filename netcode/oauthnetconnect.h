#ifndef OAUTHNETCONNECT_H
#define OAUTHNETCONNECT_H

#include <unistd.h>
#include <QtCore>
#include <QObject>
#include <QtNetwork>
#include <QApplication>
#include <QDesktopServices>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>

class OAuthNetConnect : public QObject
{
    Q_OBJECT
public:
    //ctors
    explicit OAuthNetConnect(QObject *parent);
    explicit OAuthNetConnect(const QString &scope, const QString &address, const QString &credentialFilePath, QObject *parent);
    //dtors
    ~OAuthNetConnect();
    //members
    //functions
    void buildOAuth(const QString &scope, const QString &address, const QString &credentialFilePath);
    QByteArray get();
    bool isWaitingForOauth();

private:
    //members
    QOAuth2AuthorizationCodeFlow *oauth2NetworkAccess = new QOAuth2AuthorizationCodeFlow;
    QSettings *oauthSettings;
    QString oauthToken;
    QString address;
    QDateTime tokenExpire;
    bool waitingForOauth = false;

    //functions
    QJsonObject readJsonCredentials(const QString &credentialFilePath);
    void loadSettings();
    void saveSettings();

    void oauthGranted();
    void oauthFailed();

    void debugReply();
signals:


public slots:
};

#endif // OAUTHNETCONNECT_H
