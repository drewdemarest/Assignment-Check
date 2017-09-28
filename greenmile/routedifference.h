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
    QString masterRouteDriverName;
    QString masterRouteDriverID;

    QString greenmileTruck;
    QString greenmileDriverName;
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
        qDebug() << "masterRouteDriverName" << this->masterRouteDriverName;
        qDebug() << "masterRouteDriverID" << this->masterRouteDriverID;
        qDebug() << "greenmileTruck" << this->greenmileTruck;
        qDebug() << "greenmileDriverName" << this->greenmileDriverName;
        qDebug() << "greenmileDriverID" << this->greenmileDriverID;
        qDebug() << "---------------";
    }

    bool operator == (const RouteDifference &other)
    {
        if(this->hasDiscrepencies           == other.hasDiscrepencies           &&
           this->routeExistsInGreenmile     == other.routeExistsInGreenmile     &&
           this->routeExistsInMasterRoute   == other.routeExistsInMasterRoute   &&
           this->driverMismatch             == other.driverMismatch             &&
           this->truckMismatch              == other.truckMismatch              &&
           this->routeKey                   == other.routeKey                   &&
           this->masterRouteTruck           == other.masterRouteTruck           &&
           this->masterRouteDriverName      == other.masterRouteDriverName      &&
           this->masterRouteDriverID        == other.masterRouteDriverID        &&
           this->greenmileTruck             == other.greenmileTruck             &&
           this->greenmileDriverName        == other.greenmileDriverName        &&
           this->greenmileDriverID          == other.greenmileDriverID)
        {
           return true;
        }
        else
            return false;
    }
};

#endif // ROUTEDIFFERENCE_H

