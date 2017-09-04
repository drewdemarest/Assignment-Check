#ifndef NETCONNECT_H
#define NETCONNECT_H


#include <QObject>
#include <QtNetwork>


class NetConnect : public QObject
{
    Q_OBJECT
public:
    explicit NetConnect(QObject *parent = nullptr);
    ~NetConnect();

    QByteArray postRequest(const QStringList &headers,const QString &address, const QString &body);
    void googleSheetsRead(const QString &scope, const QString &credentialFilePath);

private:


signals:
    void networkError(QString);

public slots:

};

#endif // NETCONNECT_H
