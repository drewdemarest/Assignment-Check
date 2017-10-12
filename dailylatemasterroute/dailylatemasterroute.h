#ifndef DAILYLATEMASTERROUTE_H
#define DAILYLATEMASTERROUTE_H

#include "masterroute/route.h"
#include "masterroute/routestarttime.h"
#include "greenmile/greenmile.h"
#include <algorithm>
#include <QApplication>
#include <QtCore>
#include <QObject>

class DailyLateMasterRoute : public QObject
{
        Q_OBJECT
public:
    explicit DailyLateMasterRoute(QObject *parent = nullptr);
    ~DailyLateMasterRoute();

    void buildRoutes();
    QVector<Route> getRoutes();
    QVector<RouteDifference> compareRoutesToGreenmile();

    void setRouteInfoPrecedence(QStringList &routeInfoPrecedence);
    void setEmployeeInfoPrecedence(QStringList &employeeInfoPrecedence);

private:
    //-------------------------------------------------------------------------
    // OAuth2 Network connection. This is a pointer because it
    // needs to stick around.
    //-------------------------------------------------------------------------
    //OAuthNetConnect *oauthConn_ = new OAuthNetConnect(this);
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Network settings.
    //-------------------------------------------------------------------------
    const QString sheetsScope_ =
            "https://www.googleapis.com/auth/spreadsheets.readonly";

    const QString sheetsRouteAddress_ =
            "https://sheets.googleapis.com/v4/spreadsheets/"
            "1EVbo82066Xlan6AYMlEONAaKkuwr-ejRH0f__djDpNQ/values/Today";

    const QString sheetsEmployeeAddress_ = "https://sheets.googleapis.com/v4/"
            "spreadsheets/1EVbo82066Xlan6AYMlEONAaKkuwr-ejRH0f__djDpNQ/values/"
            "Employees";

    const QString sheetsCredFilePath_ =
            QApplication::applicationDirPath() + "/client.json";

    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Offset data for the route sheets
    //-------------------------------------------------------------------------
    const QStringList defaultRouteInfoPrecedence_ {"route",
                                                   "driver",
                                                   "timeIn",
                                                   "timeOut",
                                                   "powerUnit"};

    enum mandatoryRouteFields {
        routeField,
        driverField,
        powerUnitField,
        trailerField
    };

    int routeOffset_ = 0;
    int driverOffset_ = 1;
    int timeInOffset_ = 2;
    int timeOutOffset_ = 3;
    int powerUnitOffset_ = 4;
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Employee sheet parsing data
    //-------------------------------------------------------------------------

    const QStringList defaultEmployeePrecedence_ {"blank",
                                                  "employee",
                                                  "blank",
                                                  "employeeNum"};

    enum employeeCol {
        employeeNameCol,
        employeeNumCol
    };

    QVector<int> mandatoryEmployeeColumns_ {1, 3};
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Functions
    //-------------------------------------------------------------------------
    QVector<Route> buildTodaysRoutes();
    QVector<Route> extractRoutesFromSheetValues(const QJsonArray &sheetValues);
    QMap<QString, QString> buildEmployees();
    QVector<Route> applyEmployeeNumsToRoutes(QVector<Route> routes);
    QByteArray queryRoutes();
    QByteArray queryEmployees();
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Once the sheets have been parsed they, are stored in these containers.
    //-------------------------------------------------------------------------
    QVector<Route> routes_;
    QMap<QString, QString>  employees_;

    //-------------------------------------------------------------------------
    //Helpful Route parsing utilities.
    //-------------------------------------------------------------------------
    void whatRouteFieldIsMissing(QVector<int> employeeColumnsVerify);
    void whatEmployeeColIsMissing(QVector<int> employeeColumnsVerify);

    //-------------------------------------------------------------------------
    // For specialised comparisons with greenmile. Should be able to get
    // differences from the DLMRS.
    //-------------------------------------------------------------------------
    Greenmile *gm_ = new Greenmile(this);
    //qint64 dailyLateRouteBeginOffset_ = 16200;
    QDateTime dailyLateRoutesBegin_;
    QDateTime dailyLateRoutesEnd_;


public slots:
    void abort();
    void tryNetworkAgain();
};

#endif // DAILYLATEMASTERROUTE_H
