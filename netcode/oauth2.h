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

    bool commitSettingsToDatabase(/*very long list of arguments*/);
private:

signals:

public slots:
};

#endif // OAUTH2_H
