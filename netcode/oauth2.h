#ifndef OAUTH2_H
#define OAUTH2_H

#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QApplication>
#include <QObject>
#include <QMessageBox>
#include <QSqlError>
#include <QOAuth2AuthorizationCodeFlow>

class OAuth2 : public QObject
{
    Q_OBJECT
public:
    explicit OAuth2(QObject *parent = nullptr);
    explicit OAuth2(QString dbPath, QObject *parent = nullptr);

    bool setCredentialsFromJson(QString jsonCredPath);


    QByteArray get(/*settings db path*/);

private:
    //-------------------------------------------------------------------------
    // Settings Subsection
    //-------------------------------------------------------------------------
    //Sqlite3 Database path and name
    QString dbPath_ = QApplication::applicationDirPath() + "/oauth2Settings.db";

    //Query Settings
    QString queryRequestUrl_;

    //Most OAuth2 implementations require a scope
    QString apiScope_;

    //OAuth2 Settings
    QUrl clientId_;
    QString projectId_;
    QUrl authUri_;
    QUrl tokenUri_;
    QString authProviderX509CertUrl_;
    QString clientSecret_;
    QUrl redirectUri_;
    qint16 port_;

    //

    //Functions
    bool saveSettings(QString dbPath);
    bool loadSettings(QString dbPath);
    QJsonObject makeJsonFromFile(QString jsonCredentialPath);
    //-------------------------------------------------------------------------
    // End Settings Subsection
    //-------------------------------------------------------------------------

signals:

public slots:
};

#endif // OAUTH2_H
