#ifndef ROUTEDIFFERENCE_H
#define ROUTEDIFFERENCE_H

#include <QtCore>

struct RouteDifference
{
    RouteDifference();

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

    //-------------------------------------------------------------------------
    //Operator Overloads
    //-------------------------------------------------------------------------
    bool operator==(const RouteDifference & other);
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Functions
    //-------------------------------------------------------------------------
    void printDebug() const;
    //-------------------------------------------------------------------------

};

#endif // ROUTEDIFFERENCE_H

