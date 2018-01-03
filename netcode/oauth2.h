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

    bool setCredentialsFromJsonFile(QString jsonCredPath);


    QByteArray get(/*settings db path*/);

private:
    //-------------------------------------------------------------------------
    // Settings Subsection
    //-------------------------------------------------------------------------
    //All keys in this map are lower camel...
    //This is to match up with the google paradigm, not to troll you.
    //Project must balance a little Google and also Qt.

    QMap<QString, QVariant> oauth2Settings_{{"client_id", QVariant()},
                                           {"auth_uri", QVariant()},
                                           {"token_uri", QVariant()},
                                           {"auth_provider_x509_cert_url", QVariant()},
                                           {"project_id", QVariant()},
                                           {"client_secret", QVariant()},
                                           {"query_url", QVariant()},
                                           {"api_scope", QVariant()},
                                           {"db_path", QVariant(QString(QApplication::applicationDirPath() + "/oauth2Settings.db"))},
                                           {"redirect_uris", QVariant()}};

    QVariant &dbPath_ = oauth2Settings_["db_path"];

    //Query Settings
    QString queryRequestUrl_;

    //Most OAuth2 implementations require a scope
    QString apiScope_;

    //OAuth2 Settings
    QUrl clientId_;
    QString projectId_;
    QUrl authUri_;
    QUrl tokenUri_;
    QUrl authProviderX509CertUrl_;
    QString clientSecret_;
    QUrl redirectUri_;
    qint16 port_;

    //Functions
    bool doesDatabaseExist(QString dbPath);
    bool makeInitalDatabase(QString dbPath);
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
