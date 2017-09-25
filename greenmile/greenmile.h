#ifndef GREENMILE_H
#define GREENMILE_H

#include <QApplication>
#include <QtCore>
#include <QObject>
#include "masterroute/route.h"
#include "netcode/netconnect.h"

class Greenmile : public QObject
{
    Q_OBJECT
public:
    explicit Greenmile(QObject *parent = nullptr);
    ~Greenmile();

    QVector<Route> compareRoutesToGreenmileRoutes(const QVector<Route> &otherRoutes);

private:
    void makeTimeIntervalForQuery(const QVector<Route> &r);
    QByteArray queryGreenmile(const QDateTime &begin, const QDateTime &end); 
    void buildRoutesFromGreenmileResponse(const QByteArray &gmResponse);

    void loadHeadersFromJson();
    void saveHeadersToJson();

    NetConnect *greenmileConnection = new NetConnect();
    QString headersFilePath = QApplication::applicationDirPath() + "/headers.json";

    QString gmRouteIntervalAddress = "https://charliesproduce.greenmile.com/Route/restrictions?criteria={\"filters\":[\"key\", \"driverAssignments.driver.key\", \"driverAssignments.driver.name\", \"equipmentAssignments.equipment.key\"]}";
    QString body;
    QStringList headers;
    QVector<Route> routes;

    QDateTime minQueryDateTime;
    QDateTime maxQueryDateTime;

signals:

public slots:
};

#endif // GREENMILE_H
