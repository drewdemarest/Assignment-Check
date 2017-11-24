#ifndef OAUTH2_H
#define OAUTH2_H

#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QApplication>
#include <QObject>
#include <QMessageBox>
#include <QSqlError>

class OAuth2 : public QObject
{
    Q_OBJECT
public:
    explicit OAuth2(QObject *parent = nullptr);
    QByteArray get(/*settings db path*/);

    bool commitSettingsToDatabase(QString path, QString sheetsScope,\
                                  QString sheetAddress, QString clientId,\
                                  QString projectId, QString authUri,\
                                  QString tokenUri, QString auth_providerx509,\
                                  QString clientSecret, QString redirectUris,\
                                  QString databaseFileName);
private:
    QString dbPath_ = QApplication::applicationDirPath() + "/";
    QString dbName_ = "oauth2Settings.db";

    bool saveSettings(QString dbPath);
    bool loadSettings(QString dbPath);
    //could use a QVector<QMap<QString, QVariant>> to represent a table.

signals:

public slots:
};

#endif // OAUTH2_H
