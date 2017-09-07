#ifndef ROUTE_H
#define ROUTE_H

#include <QRegExp>
#include <QtCore>


struct Route
{
    QString routeKey;
    QString truckNumber;
    QString trailerNumber;
    QString driverName;
    QString driverNumber;
    QString notes;
};

#endif // ROUTE_H
