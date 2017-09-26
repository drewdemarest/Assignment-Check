#ifndef ROUTEDIFFERENCE_H
#define ROUTEDIFFERENCE_H

#include <QtCore>

struct RouteDifference
{
    bool hasDiscrepencies = false;
    bool routeExistsInGreenmile = true;
    bool routeExistsInMasterRoute = true;
    bool driverMismatch = false;
    bool truckMismatch = false;

    QString routeKey;

    QString masterRouteTruck;
    QString masterRouteDriverID;

    QString greenmileTruck;
    QString greenmileDriverID;

    void printDebug() const
    {
        qDebug() << "---------------";
        qDebug() << "routeKey" << this->routeKey;
        qDebug() << "hasDiscrepencies" << this->hasDiscrepencies;
        qDebug() << "routeExistsInGreenmile" << this->routeExistsInGreenmile;
        qDebug() << "routeExistsInMasterRoute" << this->routeExistsInMasterRoute;
        qDebug() << "driverMismatch" << this->driverMismatch;
        qDebug() << "truckMismatch" << this->truckMismatch;
        qDebug() << "masterRouteTruck" << this->masterRouteTruck;
        qDebug() << "masterRouteDriverID" << this->masterRouteDriverID;
        qDebug() << "greenmileTruck" << this->greenmileTruck;
        qDebug() << "greenmileDriverID" << this->greenmileDriverID;
        qDebug() << "---------------";
    }
};

#endif // ROUTEDIFFERENCE_H

