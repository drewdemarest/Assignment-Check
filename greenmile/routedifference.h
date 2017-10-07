#ifndef ROUTEDIFFERENCE_H
#define ROUTEDIFFERENCE_H

#include <QtCore>
#include "masterroute/route.h"

class RouteDifference
{
    friend class RouteDifferenceModel;
    friend class Greenmile;
    friend class DailyLateMasterRoute;

public:
    RouteDifference();
    //-------------------------------------------------------------------------
    // Operator Overloads
    //-------------------------------------------------------------------------
    bool operator==(const RouteDifference &rhs) const;
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Functions
    //-------------------------------------------------------------------------
    static QVector<RouteDifference> findDifferences(const QVector<Route> &routesA,
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
    static QVector<RouteDifference> compareVectorAtoB(
                            const QVector<Route> routesA,
                            const QVector<Route> routesB);
    static QVector<RouteDifference> compareVectorBtoA(
                            const QVector<Route> routesB,
                            const QVector<Route> routesA);

    static RouteDifference findDifference(const Route &routeA,
                                          const Route &routeB);

    static RouteDifference onlyExistsInA(const Route &routeA);
    static RouteDifference onlyExistsInB(const Route &routeB);

    static QVector<RouteDifference> removeRteDiffsWithoutDiscrepencies(
                    const QVector<RouteDifference> &routeDifferencesIn);

    static QVector<RouteDifference> renameRteDiffsWithDuplicateRteKeys(
                    const QVector<RouteDifference> &routeDifferencesIn);
};

#endif // ROUTEDIFFERENCE_H

