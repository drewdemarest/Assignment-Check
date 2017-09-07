#ifndef MASTERROUTE_H
#define MASTERROUTE_H

#include "route.h"
#include <QtCore>
#include <QObject>

class MasterRoute : public QObject
{
    Q_OBJECT
public:
    explicit MasterRoute(QObject *parent = nullptr);

    void buildRoutes(const QJsonObject &qjo);
    void buildRoutes(const QByteArray &qba);

    void setRouteInfoPrecedence(QStringList &routeInfoPrecedence);

    const QStringList defaultRouteInfoPrecedence = {"route", "driver" , "powerUnit", "trailer", "notes"};
private:
    int routeOffset = 0;
    int driverOffset = 1;
    int powerUnitOffset = 2;
    int trailerOffset = 3;
    int notesOffset = 4;

    QVector<Route> routes;
    QRegExp routeRegExp = QRegExp("^[A-Z]-[A-Z,0-9]{3}");
    QRegExp driverRegExp = QRegExp("(\\w')?(\\w+),\\s[A-Z]");
    QRegExp equipmentRegExp = QRegExp("\\b\\d+\\b");

    enum routeInfo {key, driver, powerUnit, trailer};

signals:

public slots:
};

#endif // MASTERROUTE_H
