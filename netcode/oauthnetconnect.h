#ifndef OAUTHNETCONNECT_H
#define OAUTHNETCONNECT_H

#include <unistd.h>
#include <stdlib.h>
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
    //-------------------------------------------------------------------------
    // ctors & dtors
    //-------------------------------------------------------------------------
    explicit OAuthNetConnect(QObject *parent);

    explicit OAuthNetConnect(
            const QString &scope,
            const QString &address,
            const QString &credentialFilePath,
            QObject *parent);

    ~OAuthNetConnect();

    //-------------------------------------------------------------------------
    // Functions
    //-------------------------------------------------------------------------
    void buildOAuth(const QString &scope, const QString &address,
                    const QString &credentialFilePath);

    QByteArray get();
    void setResponseWaitTime(int timerDuration);

private:    
    //-------------------------------------------------------------------------
    // Functions
    //-------------------------------------------------------------------------
    QJsonObject readJsonCredentials(const QString &credentialFilePath);
    void loadSettings();
    void saveSettings();
    void debugReply();

    //-------------------------------------------------------------------------
    // Members
    //-------------------------------------------------------------------------
    QOAuth2AuthorizationCodeFlow *oauth2NetworkAccess = new QOAuth2AuthorizationCodeFlow(this);
    QTimer *responseTimer = new QTimer(this);
    int timerDuration = 10000;
    QSettings *oauthSettings;
    QString oauthToken;
    QDateTime tokenExpire;
    QString address;
    bool waitingForOauth = false;
    bool oauthValid = false;
    bool aborted = false;

signals:
    void succeeded();
    void failed();
public slots:
    void oauthGranted();
    void oauthFailed();
    void abort(bool abortStatus);
};

#endif // OAUTHNETCONNECT_H
