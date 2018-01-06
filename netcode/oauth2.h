#ifndef OAUTH2_H
#define OAUTH2_H


#include <QtCore>
#include <QObject>
#include <QMessageBox>
#include <QOAuth2AuthorizationCodeFlow>
#include "json2sqlite/json2sqlite.h"

class OAuth2 : public QObject, public Json2Sqlite
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
    QJsonObject oauth2Settings_{{"client_id", QJsonValue()},
                                {"auth_uri", QJsonValue()},
                                {"token_uri", QJsonValue()},
                                {"auth_provider_x509_cert_url", QJsonValue()},
                                {"project_id", QJsonValue()},
                                {"client_secret", QJsonValue()},
                                {"query_url", QJsonValue()},
                                {"api_scope", QJsonValue()},
                                {"db_path", QJsonValue(QString(QApplication::applicationDirPath() + "/oauth2Settings.db"))},
                                {"redirect_uris", QJsonArray()},
                                {"refresh_token", QJsonValue()}};

    //Functions
    QString dbPath_ = QApplication::applicationDirPath() + "/oauth2Settings.db";
    QJsonObject makeJsonFromFile(QString jsonCredentialPath);
    //-------------------------------------------------------------------------
    // End Settings Subsection
    //-------------------------------------------------------------------------

signals:

public slots:
};

#endif // OAUTH2_H
