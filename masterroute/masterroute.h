#ifndef MASTERROUTE_H
#define MASTERROUTE_H

#include "route.h"
#include "routestarttime.h"
#include "netcode/oauthnetconnect.h"
#include <algorithm>
#include <QApplication>
#include <QtCore>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

//namespace mandatoryDataCol
//{}

//namespace manadatoryRouteField
//{}

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

    QVector<Route> getMondayRoutes() const;
    QVector<Route> getTuesdayRoutes() const;
    QVector<Route> getWednesdayRoutes() const;
    QVector<Route> getThursdayRoutes() const;
    QVector<Route> getFridayRoutes() const;
    QVector<Route> getSaturdayRoutes() const;
    QVector<Route> getSundayRoutes() const;

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
    QString sheetsScope =
            "https://www.googleapis.com/auth/spreadsheets.readonly";

    QString sheetsAddressBase =
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

    enum mandatoryRouteFields {
        routeField,
        driverField,
        powerUnitField,
        trailerField
    };

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
    // Functions
    //-------------------------------------------------------------------------
    QDateTime extractSheetDate(const QJsonArray &sheetValues);
    QVector<Route> buildRoutes(QString dayOfWeek);
    QVector<Route> extractRoutesFromSheetValues
        (const QJsonArray &sheetValues);

    QVector<RouteStartTime> buildRouteStartTimes();
    QVector<RouteStartTime> extractRouteStartTimesFromSheetValues
        (const QJsonArray &sheetValues);

    QVector<Route> applyStartTimeToRoutes(QVector<Route> routes);
    QMap<QString, QString> buildEmployees();
    QVector<Route> applyEmployeeNumsToRoutes(QVector<Route> routes);
    QByteArray queryRoutes(QString& dayOfWeekToQuery);
    QByteArray queryRouteStartTimes();
    QByteArray queryEmployees();

    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Once the sheets have been parsed they, are stored in these containers.
    //-------------------------------------------------------------------------
    QVector<Route> mondayRoutes_;
    QVector<Route> tuesdayRoutes_;
    QVector<Route> wednesdayRoutes_;
    QVector<Route> thursdayRoutes_;
    QVector<Route> fridayRoutes_;
    QVector<Route> saturdayRoutes_;
    QVector<Route> sundayRoutes_;

    //-------------------------------------------------------------------------
    //Route parsing utilities.
    //-------------------------------------------------------------------------
    QString dateFormat = "d-MMM-yyyy";

    void whatRouteFieldIsMissing(QVector<int> routeFieldVerify);
    void whatRouteStartTimeColIsMissing(QVector<int> startTimeColumnsVerify);
    void whatEmployeeColIsMissing(QVector<int> employeeColumnsVerify);

    //-------------------------------------------------------------------------
    //Betadase things for settings
    //-------------------------------------------------------------------------
    QString settingsPath_ = QApplication::applicationDirPath() + "/settings.db";
    void loadSettingsFromDatabase();

public slots:
    void abort();
    void tryNetworkAgain();
};

#endif // MASTERROUTE_H
