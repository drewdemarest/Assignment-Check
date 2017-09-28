#ifndef DAILYLATEMASTERROUTE_H
#define DAILYLATEMASTERROUTE_H

#include "masterroute/route.h"
#include "masterroute/routestarttime.h"
#include "netcode/oauthnetconnect.h"
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

    void setRouteInfoPrecedence(QStringList &routeInfoPrecedence);
    void setEmployeeInfoPrecedence(QStringList &employeeInfoPrecedence);

private:
    bool ignoreEmptyRoutes = false;
    QString todaySheetTitle = "Today";

    //-------------------------------------------------------------------------
    // OAuth2 Network connection. This is a pointer because it
    // needs to stick around.
    //-------------------------------------------------------------------------
    OAuthNetConnect *oauthConn = new OAuthNetConnect(this);
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Network settings.
    //-------------------------------------------------------------------------
    const QString sheetsScope =
            "https://www.googleapis.com/auth/spreadsheets.readonly";

    const QString sheetsRouteAddress =
            "https://sheets.googleapis.com/v4/spreadsheets/"
            "1EVbo82066Xlan6AYMlEONAaKkuwr-ejRH0f__djDpNQ/values/Today";

    const QString sheetsEmployeeAddress = "https://sheets.googleapis.com/v4/"
                                          "spreadsheets/1EVbo82066Xlan6AYMlEONAaKkuwr-ejRH0f__djDpNQ/values/"
                                          "Employees";

    const QString sheetsCredFilePath =
            QApplication::applicationDirPath() + "/client.json";

    bool networkProblem = false;
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Offset data for the route sheets
    //-------------------------------------------------------------------------
    const QStringList defaultRouteInfoPrecedence {"route", "driver", "timeIn", "timeOut", "powerUnit"};

    int routeOffset = 0;
    int driverOffset = 1;
    int timeInOffset = 2;
    int timeOutOffset = 3;
    int powerUnitOffset = 4;
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Employee sheet parsing data
    //-------------------------------------------------------------------------

    const QStringList defaultEmployeePrecedence {"blank", "employee", "blank", "employeeNum"};

    enum employeeCol {
        employeeNameCol,
        employeeNumCol
    };

    QVector<int> mandatoryEmployeeColumns {1, 3};
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Prebuilt regex helpers
    //-------------------------------------------------------------------------
    enum regexType {matchSheetDate, matchRoute, matchDriver, matchEquipment, matchTime};
    QRegExp mrsSheetDateRegExp  = QRegExp("\\d+-[A-Z,a-z]{3}-\\d+");
    QRegExp routeRegExp         = QRegExp("^[A-Z]-[A-Z,0-9]{3}");
    QRegExp driverRegExp        = QRegExp("(\\w')?(\\w+),\\s[A-Z]");
    QRegExp equipmentRegExp     = QRegExp("\\b\\d+\\b");
    QRegExp timeRegExp          = QRegExp("\\d+:\\d+)?(\\d){4,3}\\s*\\w+");
    //Added all regexp to vector allow for more succinct code.
    QVector<QRegExp> regExpVector {mrsSheetDateRegExp, routeRegExp, driverRegExp, equipmentRegExp};
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Functions
    //-------------------------------------------------------------------------
    void buildEmployees();
    QVector<Route> applyEmployeeNumsToRoutes(QVector<Route> routes);
    QByteArray queryRoutes();
    QByteArray queryEmployees();
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Once the sheets have been parsed they, are stored in these containers.
    //-------------------------------------------------------------------------
    QVector<RouteStartTime> routeStartTimes;
    QVector<Route> routes;
    QMap<QString, QString>  employees;

    //-------------------------------------------------------------------------
    //Route parsing utilities.
    //-------------------------------------------------------------------------
    QString dateFormat = "d-MMM-yyyy";

    void whatRouteColIsMissing();
    void whatEmployeeColIsMissing(QVector<int> employeeColumnsVerify);

public slots:
    void abort();
    void tryNetworkAgain();
};

#endif // DAILYLATEMASTERROUTE_H
