#ifndef ROUTEDIFFERENCE_H
#define ROUTEDIFFERENCE_H

#include <QtCore>
#include "masterroute/route.h"

class RouteDifference
{
    friend class RouteDifferenceModel;

public:
    RouteDifference();
    //-------------------------------------------------------------------------
    // Operator Overloads
    //-------------------------------------------------------------------------
    bool operator==(const RouteDifference &rhs);
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Functions
    //-------------------------------------------------------------------------
    QVector<RouteDifference> findDifferences(const QVector<Route> &routesA,
                                             const QVector<Route> &routesB);

    void printDebug() const;
    //-------------------------------------------------------------------------

private:
    bool hasDiscrepencies = false;
    bool routeExistsInSourceA = true;
    bool routeExistsInSourceB = true;
    bool driverMismatch = false;
    bool truckMismatch = false;

    QString routeKey;

    QString powerUnitA;
    QString driverNameA;
    QString driverNumberA;

    QString powerUnitB;
    QString driverNameB;
    QString driverNumberB;

    //-------------------------------------------------------------------------
    // Functions
    //-------------------------------------------------------------------------
    QVector<RouteDifference> compareVectorAtoB(
            const QVector<Route> routesA,
            const QVector<Route> routesB);
    QVector<RouteDifference> compareVectorBtoA(
            const QVector<Route> routesB,
            const QVector<Route> routesA);

    RouteDifference findDifference(const Route &routeA,
                                   const Route &routeB);

    RouteDifference onlyExistsInA(const Route &routeA);
    RouteDifference onlyExistsInB(const Route &routeB);

};

#endif // ROUTEDIFFERENCE_H

