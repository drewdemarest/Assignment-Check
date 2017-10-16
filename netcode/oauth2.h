#ifndef OAUTH2_H
#define OAUTH2_H

#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QApplication>
#include <QObject>

class OAuth2 : public QObject
{
    Q_OBJECT
public:
    explicit OAuth2(QObject *parent = nullptr);
    QByteArray get(/*settings db path*/);
    QSqlDatabase makeDatabase(/*very long list of arguments*/);    

private:

signals:

public slots:
};

#endif // OAUTH2_H
