#ifndef GREENMILE_H
#define GREENMILE_H

#include <QApplication>
#include <QtCore>
#include <QObject>
#include "masterroute/route.h"
#include "routedifference.h"
#include "netcode/netconnect.h"

class Greenmile : public QObject
{
    Q_OBJECT
public:
    enum TimeIntervalError {noError, nullInterval, minGteMax};

    explicit Greenmile(QObject *parent = nullptr);
    ~Greenmile();

    QVector<Route> getRoutesForTimeInterval(const QDateTime &minQueryDateTime, const QDateTime &maxQueryDateTime);
    QVector<RouteDifference> compareRoutesToGreenmileRoutes(const QVector<Route> &otherRoutes);

private:
    TimeIntervalError timeIntervalHasError(const QDateTime &minQueryDateTime, const QDateTime &maxQueryDateTime);
    void makeTimeIntervalForQuery(const QVector<Route> &r);
    QByteArray queryGreenmile(const QDateTime &begin, const QDateTime &end); 
    QVector<Route> buildRoutesFromGreenmileResponse(const QByteArray &gmResponse);

    void loadHeadersFromJson();
    void saveHeadersToJson();

    NetConnect *greenmileConnection_ = new NetConnect();
    QString headersFilePath_ = QApplication::applicationDirPath() + "/headers.json";

    qint64 timeIntervalPaddingMsec_ = 0;
    QString gmRouteIntervalAddress_ = "https://charliesproduce.greenmile.com/Route/restrictions?criteria={\"filters\":[\"key\", \"driverAssignments.driver.key\", \"driverAssignments.driver.name\", \"equipmentAssignments.equipment.key\"]}";
    QString body_;
    QStringList headers_;
    QVector<Route> routes_;
    QVector<RouteDifference> routeDifferences_;

    QDateTime minQueryDateTime_;
    QDateTime maxQueryDateTime_;

    QDateTime minDLMRSDateTime_;
    QDateTime maxDLMRSDateTime_;

signals:

public slots:
};

#endif // GREENMILE_H
