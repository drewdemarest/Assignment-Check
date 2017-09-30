#ifndef MASTERROUTE_H
#define MASTERROUTE_H

#include "route.h"
#include "routestarttime.h"
#include "netcode/oauthnetconnect.h"
#include <algorithm>
#include <QApplication>
#include <QtCore>
#include <QObject>

class MasterRoute : public QObject
{
    Q_OBJECT
public:
    explicit MasterRoute(QObject *parent = nullptr);   
    ~MasterRoute();

    void buildAllRoutes();
    void buildMondayRoutes();
    void buildTuesdayRoutes();
    void buildWednesdayRoutes();
    void buildThursdayRoutes();
    void buildFridayRoutes();
    void buildSaturdayRoutes();
    void buildSundayRoutes();

    QVector<Route> getMondayRoutes();
    QVector<Route> getTuesdayRoutes();
    QVector<Route> getWednesdayRoutes();
    QVector<Route> getThursdayRoutes();
    QVector<Route> getFridayRoutes();
    QVector<Route> getSaturdayRoutes();
    QVector<Route> getSundayRoutes();

    void setRouteInfoPrecedence(QStringList &routeInfoPrecedence);
    void setStartTimeInfoPrecedence(QStringList &startTimeInfoPrecedence);
    void setEmployeeInfoPrecedence(QStringList &employeeInfoPrecedence);

private:
    QString mondaySheetTitle = "Monday";
    QString tuesdaySheetTitle = "Tuesday";
    QString wednesdaySheetTitle = "Wednesday";
    QString thursdaySheetTitle = "Thursday";
    QString fridaySheetTitle = "Friday";
    QString saturdaySheetTitle = "Saturday";
    QString sundaySheetTitle = "Sunday";

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

    const QString sheetsAddressBase =
            "https://sheets.googleapis.com/v4/spreadsheets/"
            "1KA7c9bbG2p4f8SFe5ibbkIycwt0wukRe2_xpTB3SI6A/values/";

    const QString sheetsEmployeeAddress = "https://sheets.googleapis.com/v4/"
                                          "spreadsheets/1KA7c9bbG2p4f8SFe5ibb"
                                          "kIycwt0wukRe2_xpTB3SI6A/values/"
                                          "Employees";


    const QString sheetsStartTimeAddress = "https://sheets.googleapis.com/v4/"
                                           "spreadsheets/1KA7c9bbG2p4f8SFe5ibb"
                                           "kIycwt0wukRe2_xpTB3SI6A/values/"
                                           "Route Start Times";

    const QString sheetsCredFilePath =
            QApplication::applicationDirPath() + "/client.json";

    bool networkProblem = false;
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Offset data for the route sheets
    //-------------------------------------------------------------------------
    const QStringList defaultRouteInfoPrecedence {"route", "driver",
                                                  "powerUnit", "trailer"};

    int routeOffset = 0;
    int driverOffset = 1;
    int powerUnitOffset = 2;
    int trailerOffset = 3;
    int notesOffset = 4;
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Offset data for the route start time sheet
    //-------------------------------------------------------------------------
    const QStringList defaultRouteStartTimePrecedence {"building", "route",
                                                       "name", "startsPrevDay",
                                                       "mon", "tue", "wed",
                                                       "thu", "fri", "sat",
                                                       "sun"};

    enum routeStartTimeCol {
        routeKeyStartTimeCol,
        startsPrevDayStartTimeCol,
        monStartTimeCol,
        tueStartTimeCol,
        wedStartTimeCol,
        thuStartTimeCol,
        friStartTimeCol,
        satStartTimeCol,
        sunStartTimeCol
    };

    QVector<int> mandatoryStartTimeColumns {1, 3, 4, 5, 6, 7, 8, 9, 10};
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
    enum regexType {matchSheetDate, matchRoute, matchDriver, matchEquipment,
                    matchTime};
    QRegExp mrsSheetDateRegExp  = QRegExp("\\d+-[A-Z,a-z]{3}-\\d+");
    QRegExp routeRegExp         = QRegExp("^[A-Z]-[A-Z,0-9]{3}");
    QRegExp driverRegExp        = QRegExp("(?:\\w+-)?\\w+'?\\w+,\\s[A-Z]");
    QRegExp equipmentRegExp     = QRegExp("\\b\\d+\\b");
    QRegExp timeRegExp          = QRegExp("\\d+:\\d+|\\d{3,4}");
    //Added all regexp to vector allow for more succinct code.
    QVector<QRegExp> regExpVector {mrsSheetDateRegExp, routeRegExp,
                driverRegExp, equipmentRegExp};
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Functions
    //-------------------------------------------------------------------------
    QDateTime extractSheetDate(const QJsonArray &sheet);
    QVector<Route> buildRoutes(QString dayOfWeek);
    QVector<Route> extractRoutesFromSheet(const QJsonArray &routeArray);
    void buildRouteStartTimes();
    RouteStartTime buildRouteStartTimeFromRow(const QJsonArray &row);
    QVector<Route> applyStartTimeToRoutes(QVector<Route> routes);
    void buildEmployees();
    QVector<Route> applyEmployeeNumsToRoutes(QVector<Route> routes);
    QByteArray queryRoutes(QString& dayOfWeekToQuery);
    QByteArray queryRouteStartTimes();
    QByteArray queryEmployees();

    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Once the sheets have been parsed they, are stored in these containers.
    //-------------------------------------------------------------------------
    QVector<RouteStartTime> routeStartTimes;
    QVector<Route>          mondayRoutes;
    QVector<Route>          tuesdayRoutes;
    QVector<Route>          wednesdayRoutes;
    QVector<Route>          thursdayRoutes;
    QVector<Route>          fridayRoutes;
    QVector<Route>          saturdayRoutes;
    QVector<Route>          sundayRoutes;
    QMap<QString, QString>  employees;

    //-------------------------------------------------------------------------
    //Route parsing utilities.
    //-------------------------------------------------------------------------
    QString dateFormat = "d-MMM-yyyy";

    void whatRouteColIsMissing();
    void whatRouteStartTimeColIsMissing(QVector<int> startTimeColumnsVerify);
    void whatEmployeeColIsMissing(QVector<int> employeeColumnsVerify);

public slots:
    void abort();
    void tryNetworkAgain();
};

#endif // MASTERROUTE_H
