#ifndef MASTERROUTE_H
#define MASTERROUTE_H

#include <QtCore>
#include <QObject>

class MasterRoute : public QObject
{
    Q_OBJECT
public:
    explicit MasterRoute(QObject *parent = nullptr);

    void buildRoutes(const QJsonObject &qjo);
    void buildRoutes(const QByteArray &qba);
signals:

public slots:
};

#endif // MASTERROUTE_H
