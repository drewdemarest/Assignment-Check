#ifndef OAUTH2_H
#define OAUTH2_H


#include <QtCore>
#include <QObject>
#include <QMessageBox>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QAbstractOAuth2>
#include <QDesktopServices>
#include <QNetworkReply>
#include "json2sqlite/json2sqlite.h"
#include <unistd.h>

class OAuth2 : public QObject, public Json2Sqlite
{
    Q_OBJECT
public:
    explicit OAuth2(QString dbPath,
                    QString client_id,
                    QString project_id,
                    QString auth_uri,
                    QString token_uri,
                    QString auth_provider_x509_cert_url,
                    QString client_secret,
                    QStringList redirect_uris,
                    QString scope = QString(),
                    QObject *parent = nullptr);

    explicit OAuth2(QString dbPath,
                    QString googleJsonCredPath,
                    QString scope = QString(),
                    QObject *parent = nullptr);

    explicit OAuth2(QString dbPath,
                    QString scope = QString(),
                    QObject *parent = nullptr);

    bool setCredentialsFromJsonFile(const QString &jsonCredPath);
    void setScope(const QString &scope);

    QByteArray get(const QString &address);

private:
    //-------------------------------------------------------------------------
    // Settings Subsection
    //-------------------------------------------------------------------------
    //All keys in this map are lower camel...
    //This is to match up with the google paradigm, not to troll you.
    //Project must balance a little Google and also Qt.
    bool readyForRequests_ = false;
    QJsonObject oauth2Settings_{{"client_id", QJsonValue()},
                                {"auth_uri", QJsonValue()},
                                {"token_uri", QJsonValue()},
                                {"auth_provider_x509_cert_url", QJsonValue()},
                                {"project_id", QJsonValue()},
                                {"client_secret", QJsonValue()},
                                {"query_url", QJsonValue()},
                                {"api_scope", QJsonValue()},
                                {"db_path", QJsonValue(QString(QApplication::applicationDirPath() + "/oauth2Settings.db"))},
                                {"json_credential_path", QJsonValue()},
                                {"redirect_uris", QJsonArray()},
                                {"token", QJsonValue()},
                                {"expiration_at", QJsonValue()},
                                {"refresh_token", QJsonValue()}};

    //Functions
    QString dbPath_ = QApplication::applicationDirPath() + "/oauth2Settings.db";
    QJsonObject makeJsonFromFile(QString jsonCredentialPath);
    bool build();

    QOAuth2AuthorizationCodeFlow *google = new QOAuth2AuthorizationCodeFlow;
    //-------------------------------------------------------------------------
    // End Settings Subsection
    //-------------------------------------------------------------------------

signals:

public slots:

private slots:
    bool saveOAuth2TokensToDB();
    bool tokens(const QByteArray &tokens);

};

#endif // OAUTH2_H
