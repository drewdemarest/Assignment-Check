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
    void setTimerDuration(int timerDuration);

private:
    //members
    QTimer *responseTimer = new QTimer(this);
    QOAuth2AuthorizationCodeFlow *oauth2NetworkAccess = new QOAuth2AuthorizationCodeFlow(this);
    QSettings *oauthSettings;
    QString oauthToken;
    QString address;
    QDateTime tokenExpire;
    bool waitingForOauth = false;
    bool oauthValid = false;

    //functions
    QJsonObject readJsonCredentials(const QString &credentialFilePath);
    void loadSettings();
    void saveSettings();

    void debugReply();
    int timerDuration = 30000;
    bool isDeleted = false;
signals:


public slots:
    void oauthGranted();
    void oauthFailed();
};

#endif // OAUTHNETCONNECT_H
