#ifndef MASTERROUTE_H
#define MASTERROUTE_H

#include "route.h"
#include "netcode/oauthnetconnect.h"
#include <QApplication>
#include <QtCore>
#include <QObject>

class MasterRoute : public QObject
{
    Q_OBJECT
public:
    explicit MasterRoute(QObject *parent = nullptr);

    void buildRoutes();
    void setRouteInfoPrecedence(QStringList &routeInfoPrecedence);
    void setStartTimeInfoPrecedence(QStringList &startTimeInfoPrecedence);

private:
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
            "10mbrfPlPiTi991BKM0nvFa2wDjM_0WHB7AO-xFn9U3c/values/";

    const QString sheetsStartTimeAddress = "https://sheets.googleapis.com/v4/"
            "spreadsheets/10mbrfPlPiTi991BKM0nvFa2wDjM_0WHB7AO-xFn9U3c/values/"
            "Route Start Times";

    const QString sheetsCredFilePath =
            QApplication::applicationDirPath() + "/client.json";
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Offset data for the route sheets
    //-------------------------------------------------------------------------
    const QStringList defaultRouteInfoPrecedence =
    {"route", "driver" , "powerUnit", "trailer"};

    int routeOffset = 0;
    int driverOffset = 1;
    int powerUnitOffset = 2;
    int trailerOffset = 3;
    int notesOffset = 4;
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Offset data for the route start time sheet
    //-------------------------------------------------------------------------
    const QStringList defaultRouteStartTimePrecedence =
    {"building", "route", "name", "startsPrevDay", "mon",
     "tue", "wed", "thu", "fri","sat", "sun"};

    int buildingOffset      = -1;
    //routeOffset already   = 0;
    int nameOffset          = 1;
    int startsPrevDayOffset = 2;
    int monOffset           = 3;
    int tueOffset           = 4;
    int wedOffset           = 5;
    int thuOffset           = 6;
    int friOffset           = 7;
    int satOffset           = 8;
    int sunOffset           = 9;
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Useful info to know...
    //-------------------------------------------------------------------------
    const QStringList daysOfTheWeek =
    {"Monday", "Tuesday", "Wednesday", "Thursday",
     "Friday", "Saturday", "Sunday", "test"};
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Prebuilt regex helpers
    //-------------------------------------------------------------------------
    enum regexType {matchSheetDate, matchRoute, matchDriver, matchEquipment};
    QRegExp mrsSheetDateRegExp  = QRegExp("\\d+-[A-Z,a-z]{3}-\\d+");
    QRegExp routeRegExp         = QRegExp("^[A-Z]-[A-Z,0-9]{3}");
    QRegExp driverRegExp        = QRegExp("(\\w')?(\\w+),\\s[A-Z]");
    QRegExp equipmentRegExp     = QRegExp("\\b\\d+\\b");
    //Added all regexp to vector allow for more succinct code.
    const QVector<QRegExp*> regExpVector = {&mrsSheetDateRegExp, &routeRegExp,
                                     &driverRegExp, &equipmentRegExp};
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Functions
    //-------------------------------------------------------------------------
    void buildRouteStartTimes();
    QByteArray queryRoutes(QString& dayOfWeekToQuery);
    QByteArray queryRouteStartTimes();
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Once the sheets have been parsed they, are stored in these containers.
    //-------------------------------------------------------------------------
    QMap<QString, QVector<QTime>>   routeStartTimes;
    QVector<Route>                  routes;


signals:

public slots:
};

#endif // MASTERROUTE_H
