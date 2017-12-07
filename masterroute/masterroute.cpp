#include "masterroute.h"

MasterRoute::MasterRoute(QObject *parent) : QObject(parent)
{

    loadSettingsFromDatabase();

}

MasterRoute::~MasterRoute()
{
    //oauthConn->deleteLater();
}

void MasterRoute::buildAllRoutes()
{ 
    mondayRoutes_ = buildRoutes(mondaySheetTitle);
    tuesdayRoutes_ = buildRoutes(tuesdaySheetTitle);
    wednesdayRoutes_ = buildRoutes(wednesdaySheetTitle);
    thursdayRoutes_ = buildRoutes(thursdaySheetTitle);
    fridayRoutes_ = buildRoutes(fridaySheetTitle);
    saturdayRoutes_ = buildRoutes(saturdaySheetTitle);
    sundayRoutes_ = buildRoutes(sundaySheetTitle);
}

void MasterRoute::buildMondayRoutes()
{
    mondayRoutes_ = buildRoutes(mondaySheetTitle);
}

void MasterRoute::buildTuesdayRoutes()
{
    tuesdayRoutes_ = buildRoutes(tuesdaySheetTitle);
}

void MasterRoute::buildWednesdayRoutes()
{
    wednesdayRoutes_ = buildRoutes(wednesdaySheetTitle);
}

void MasterRoute::buildThursdayRoutes()
{
    thursdayRoutes_ = buildRoutes(thursdaySheetTitle);
}

void MasterRoute::buildFridayRoutes()
{
    fridayRoutes_ = buildRoutes(fridaySheetTitle);
}

void MasterRoute::buildSaturdayRoutes()
{
    saturdayRoutes_ = buildRoutes(saturdaySheetTitle);
}

void MasterRoute::buildSundayRoutes()
{
    sundayRoutes_ = buildRoutes(sundaySheetTitle);
}

QVector<Route> MasterRoute::getMondayRoutes() const
{
    return mondayRoutes_;
}

QVector<Route> MasterRoute::getTuesdayRoutes() const
{
    return tuesdayRoutes_;
}

QVector<Route> MasterRoute::getWednesdayRoutes() const
{
    return wednesdayRoutes_;
}

QVector<Route> MasterRoute::getThursdayRoutes() const
{
    return thursdayRoutes_;
}

QVector<Route> MasterRoute::getFridayRoutes() const
{
    return fridayRoutes_;
}

QVector<Route> MasterRoute::getSaturdayRoutes() const
{
    return saturdayRoutes_;
}

QVector<Route> MasterRoute::getSundayRoutes() const
{
    return sundayRoutes_;
}

QVector<Route> MasterRoute::buildRoutes(QString dayOfWeek)
{
    QVector<Route> routes;
    QJsonObject routeSheet = QJsonDocument::fromJson\
            (queryRoutes(dayOfWeek)).object();
    QJsonArray sheetValues = routeSheet["values"].toArray();

    routes = extractRoutesFromSheetValues(sheetValues);
    routes = applyStartTimeToRoutes(routes);
    routes = applyEmployeeNumsToRoutes(routes);

    std::sort(routes.begin(), routes.end(), [](Route r1, Route r2) ->\
            bool {return r1.getKey() < r2.getKey();});

    for(auto t: routes)
        qDebug() << t.getKey() << t.getRouteDate().toUTC().toString()\
                 << t.getDriverName() << t.getDriverId() << "rte dbg";

    return routes;
}

QVector<Route> MasterRoute::extractRoutesFromSheetValues(const QJsonArray &sheetValues)
{
    QDateTime sheetDate = extractSheetDate(sheetValues);

    Route route;
    QVector<Route> routes;
    QVector<int> routeKeyFoundCol;
    QVector<int> driverFoundCol;
    QVector<int> equipmentFoundCol;
    QVector<int> miscFoundCol;

    QJsonArray row;

    for(int rowCount = 0; rowCount < sheetValues.size(); rowCount++)
    {
        route = Route();
        row = sheetValues.at(rowCount).toArray();

        routeKeyFoundCol.clear();
        driverFoundCol.clear();
        equipmentFoundCol.clear();
        miscFoundCol.clear();

        for(int colCount = 0; colCount < row.size(); colCount++)
        {
            //-----------------------------------------------------------------
            // Asks the route what this cell is. If enough of these columns
            // are populated with data then a potential route has been
            // located.
            //-----------------------------------------------------------------
            switch(route.whatIsThis(row.at(colCount).toString()))
            {
            case routeEnum::matchDriverName:
                driverFoundCol.append(colCount);
                break;

            case routeEnum::matchEquipment:
                equipmentFoundCol.append(colCount);
                break;

            case routeEnum::matchMisc:
                miscFoundCol.append(colCount);
                break;

            case routeEnum::matchRouteKey:
                routeKeyFoundCol.append(colCount);
                break;
            }
        }

        //---------------------------------------------------------------------
        // For all the route keys that have been found, the rest of the
        // columns for supporting route information are determined by
        // using the expected offsets.
        // This allows a route to exist anywhere in a sheet but we're still
        // able to assemble it based to where the rest of the route info
        // should be relative to the key itself.
        //---------------------------------------------------------------------
        for(auto expectedRouteKeyCol: routeKeyFoundCol)
        {
            route = Route();

            int expectedDriverCol = expectedRouteKeyCol +  driverOffset;
            int expectedPowerUnitCol = expectedRouteKeyCol + powerUnitOffset;
            int expectedTrailerCol = expectedRouteKeyCol + trailerOffset;

            route.setField(row.at(expectedRouteKeyCol)\
                           .toString(), routeEnum::key);

            if(driverFoundCol.contains(expectedDriverCol))
            {
                route.setField(row.at(expectedDriverCol).toString(), routeEnum::driverName);
            }

            if(equipmentFoundCol.contains(expectedPowerUnitCol))
            {
                route.setField(row.at(expectedPowerUnitCol).toString(), routeEnum::truckNum);
            }

            if(equipmentFoundCol.contains(expectedTrailerCol))
            {
                route.setField(row.at(expectedTrailerCol).toString(), routeEnum::trailerNum);;
            }

            route.setField(sheetDate.toString(route.getDateFormat()), routeEnum::date);
            routes.append(route);
        }
    }
    return routes;
}

void MasterRoute::setRouteInfoPrecedence(QStringList &routeInfoPrecedence)
{
    const QVector<int> routeColumnsVerify
    {
        routeInfoPrecedence.indexOf("route"),
        routeInfoPrecedence.indexOf("driver"),
        routeInfoPrecedence.indexOf("powerUnit"),
        routeInfoPrecedence.indexOf("trailer"),
    };

    if(!routeColumnsVerify.contains(-1))
    {
        const int routeIndex        = routeColumnsVerify.at(0);
        const int driverIndex       = routeColumnsVerify.at(1);
        const int powerUnitIndex    = routeColumnsVerify.at(2);
        const int trailerIndex      = routeColumnsVerify.at(3);

        routeOffset     = routeIndex - routeIndex;
        driverOffset    = driverIndex - routeIndex;
        powerUnitOffset = powerUnitIndex - routeIndex;
        trailerOffset   = trailerIndex - routeIndex;
    }
    else
    {
        whatRouteFieldIsMissing(routeColumnsVerify);
    }
}

void MasterRoute::setStartTimeInfoPrecedence(QStringList &startTimeInfoPrecedence)
{
    QVector<int> startTimeColumnsVerify =
    {
        startTimeInfoPrecedence.indexOf("route"),
        startTimeInfoPrecedence.indexOf("startsPrevDay"),
        startTimeInfoPrecedence.indexOf("mon"),
        startTimeInfoPrecedence.indexOf("tue"),
        startTimeInfoPrecedence.indexOf("wed"),
        startTimeInfoPrecedence.indexOf("thu"),
        startTimeInfoPrecedence.indexOf("fri"),
        startTimeInfoPrecedence.indexOf("sat"),
        startTimeInfoPrecedence.indexOf("sun"),
    };

    if(!startTimeColumnsVerify.contains(-1))
    {
        mandatoryStartTimeColumns = startTimeColumnsVerify;
    }
    else
    {
        whatRouteStartTimeColIsMissing(startTimeColumnsVerify);
    }
}

void MasterRoute::setEmployeeInfoPrecedence(QStringList &employeeInfoPrecedence)
{
    QVector<int> employeeColumnsVerify =
    {
        employeeInfoPrecedence.indexOf("employeeName"),
        employeeInfoPrecedence.indexOf("employeeNum"),
    };

    if(!employeeColumnsVerify.contains(-1))
    {
        mandatoryEmployeeColumns = employeeColumnsVerify;
        qDebug() << mandatoryEmployeeColumns;
    }
    else
    {
        whatEmployeeColIsMissing(employeeColumnsVerify);
    }
}

QDateTime MasterRoute::extractSheetDate(const QJsonArray &sheetValues)
{
    QDateTime sheetDate;
    Route route;
    for(int rowCount = 0; rowCount < sheetValues.size(); rowCount++)
    {
        QJsonArray row = sheetValues.at(rowCount).toArray();
        for(int colCount = 0; colCount < row.size(); colCount++)
        {
            switch(route.whatIsThis(row.at(colCount).toString()))
            {
            case routeEnum::matchSheetDate:
                sheetDate = QDateTime::fromString(row.at(colCount)\
                                                  .toString(), dateFormat);
                return sheetDate;
            }
        }
    }
    return QDateTime();
}

QVector<RouteStartTime> MasterRoute::buildRouteStartTimes()
{
    QJsonObject sheet =
            QJsonDocument::fromJson(queryRouteStartTimes()).object();
    QJsonArray sheetValues = sheet["values"].toArray();

    return extractRouteStartTimesFromSheetValues(sheetValues);
}



QVector<RouteStartTime> MasterRoute::extractRouteStartTimesFromSheetValues(const QJsonArray &sheetValues)
{
    RouteStartTime routeStart;
    QVector<RouteStartTime> routeStartTimes;

    QString timeFormat  = "h:mm";
    int msecsInDay = 86400000;

    for(int rowCount = 1; rowCount < sheetValues.size(); rowCount++)
    {
        QJsonArray row = sheetValues.at(rowCount).toArray();

        for(int colCount = 0; colCount < row.size(); colCount++)
        {
            if(mandatoryStartTimeColumns.at(routeKeyStartTimeCol) == colCount)
                routeStart.routeKey = row.at(colCount).toString();

            if(mandatoryStartTimeColumns.at(startsPrevDayStartTimeCol) == colCount)
            {
                routeStart.startsPrevDay[routeEnum::mon] =
                        row.at(colCount).toString().contains("M");
                routeStart.startsPrevDay[routeEnum::tue] =
                        row.at(colCount).toString().contains("T");
                routeStart.startsPrevDay[routeEnum::wed] =
                        row.at(colCount).toString().contains("W");
                routeStart.startsPrevDay[routeEnum::thu] =
                        row.at(colCount).toString().contains("R");
                routeStart.startsPrevDay[routeEnum::fri] =
                        row.at(colCount).toString().contains("F");
                routeStart.startsPrevDay[routeEnum::sat] =
                        row.at(colCount).toString().contains("S");
                routeStart.startsPrevDay[routeEnum::sun] =
                        row.at(colCount).toString().contains("U");
            }

            if(mandatoryStartTimeColumns.at(monStartTimeCol) == colCount)
            {
                if(routeStart.startsPrevDay[routeEnum::mon])
                {
                    routeStart.monMidnightOffsetmSec =
                        (QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.monMidnightOffsetmSec =
                        QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(tueStartTimeCol) == colCount)
            {
                if(routeStart.startsPrevDay.at(routeEnum::tue))
                {
                    routeStart.tueMidnightOffsetmSec =
                        (QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.tueMidnightOffsetmSec =
                        QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(wedStartTimeCol) == colCount)
            {
                if(routeStart.startsPrevDay[routeEnum::wed])
                {
                    routeStart.wedMidnightOffsetmSec =
                        (QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay()) - msecsInDay;
                }
                else
                {
                    routeStart.wedMidnightOffsetmSec =
                        QTime::fromString(row.at(colCount).toString(),
                            timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(thuStartTimeCol) == colCount)
            {
                if(routeStart.startsPrevDay[routeEnum::thu])
                {
                    routeStart.thuMidnightOffsetmSec =
                        (QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);

                }
                else
                {
                    routeStart.thuMidnightOffsetmSec =
                        QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(friStartTimeCol) == colCount)
            {
                if(routeStart.startsPrevDay[routeEnum::fri])
                {
                    routeStart.friMidnightOffsetmSec =
                        (QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.friMidnightOffsetmSec =
                        QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(satStartTimeCol) == colCount)
            {
                if(routeStart.startsPrevDay[routeEnum::sat])
                {
                    routeStart.satMidnightOffsetmSec =
                        (QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.satMidnightOffsetmSec =
                        QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(sunStartTimeCol) == colCount)
            {
                if(routeStart.startsPrevDay[routeEnum::sun])
                {
                    routeStart.sunMidnightOffsetmSec =
                        (QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.sunMidnightOffsetmSec =
                        QTime::fromString(row.at(colCount).toString(),\
                            timeFormat).msecsSinceStartOfDay();
                }
            }
        }
        routeStartTimes.append(routeStart);
    }
    return routeStartTimes;
}

QVector<Route> MasterRoute::applyStartTimeToRoutes(QVector<Route> routes)
{
    QVector<RouteStartTime> routeStartTimes = buildRouteStartTimes();
    QVector<Route>::iterator start = routes.begin();
    QVector<Route>::iterator end = routes.end();

    for(auto t: routeStartTimes)
    {
        QString toFind = t.routeKey;

        start = std::find_if(start, end, [&toFind](Route &j)\
        {return j.getKey() == toFind;});

        if(start != end)
        {
            start->applyRouteStartTime(t);
        }
        start = routes.begin();
        end = routes.end();
    }
    return routes;
}

QMap<QString, QString> MasterRoute::buildEmployees()
{
    QMap<QString, QString> employees;
    QString employeeName;
    QString employeeNumber;
    QJsonObject sheet =
            QJsonDocument::fromJson(queryEmployees()).object();

    QJsonArray sheetValues = sheet["values"].toArray();
    QJsonArray row;

    for(int rowCount = 1; rowCount < sheetValues.size(); rowCount++)
    {
        row = sheetValues.at(rowCount).toArray();

        for(int colCount = 0; colCount < row.size(); colCount++)
        {
            if(mandatoryEmployeeColumns.at(employeeNameCol) == colCount)
            {
                employeeName = row.at(colCount).toString();
            }
            if(mandatoryEmployeeColumns.at(employeeNumCol) == colCount)
            {
                employeeNumber = row.at(colCount).toString();
            }

        }
        employees[employeeName] = employeeNumber;
        employeeName = QString();
        employeeNumber = QString();
    }
    return employees;
}

QVector<Route> MasterRoute::applyEmployeeNumsToRoutes(QVector<Route> routes)
{
    QMap<QString, QString> employees = buildEmployees();
    QVector<Route>::iterator routesIter = routes.begin();
    while(routesIter != routes.constEnd())
    {
        if(!routesIter->getDriverName().trimmed().isEmpty())
        {
            if(employees.contains(routesIter->getDriverName()))
            {
                routesIter->setField(employees[routesIter->getDriverName()],\
                        routeEnum::driverId);
            }
            else
            {
                qDebug() << routesIter->getKey() << "has an incorrect driver.";
            }
        }
        ++routesIter;
    }
    return routes;
}

QByteArray MasterRoute::queryRoutes(QString &dayOfWeekToQuery)
{
    /*
    oauthConn->buildOAuth(sheetsScope,\
                          QString(sheetsAddressBase + dayOfWeekToQuery),\
                          sheetsCredFilePath);

    return oauthConn->get();
    */
    return QByteArray();
}

QByteArray MasterRoute::queryRouteStartTimes()
{
//    oauthConn->buildOAuth(sheetsScope,
//                          sheetsStartTimeAddress,
//                          sheetsCredFilePath);
//    return oauthConn->get();
    return QByteArray();
}

QByteArray MasterRoute::queryEmployees()
{
//    oauthConn->buildOAuth(sheetsScope,
//                          sheetsEmployeeAddress,
//                          sheetsCredFilePath);
//    return oauthConn->get();
    return QByteArray();
}

void MasterRoute::whatRouteFieldIsMissing(QVector<int> routeFieldVerify)
{
    //iterates through vector and provides the indexes of all duplicates.
    //This is then mapped to the enum type regarding what vector idx applies
    //to what column.
    int matchNotFound = -1;
    QVector<int>::iterator start = routeFieldVerify.begin();
    QVector<int>::iterator end = routeFieldVerify.end();

    while(end > start){
        start = std::find_if(start, end, [&matchNotFound](const int& j)\
        {return j == matchNotFound;});

        if(start != end)
        {
            switch(start - routeFieldVerify.begin())
            {
            case(routeField):
                qDebug() << "routeField missing.\
                            Reverting to default precedence.";
                            break;

            case(driverField):
                qDebug() << "driverField missing.\
                            Reverting to default precedence.";
                            break;

            case(powerUnitField):
                qDebug() << "powerUnitField missing. \
                            Reverting to default precedence.";
                            break;

            case(trailerField):
                qDebug() << "trailerField missing.\
                            Reverting to default precedence.";
                            break;
            }
        }
        start++;
    }
}

void MasterRoute::whatRouteStartTimeColIsMissing\
(QVector<int> startTimeColumnsVerify)
{
    //iterates through vector and provides the indexes of all duplicates.
    //This is then mapped to the enum type regarding what vector idx applies
    //to what column.
    int matchNotFound = -1;
    QVector<int>::iterator start = startTimeColumnsVerify.begin();
    QVector<int>::iterator end = startTimeColumnsVerify.end();

    while(end > start){
        start = std::find_if(start, end, [&matchNotFound](const int& j)\
        {return j == matchNotFound;});

        if(start != end)
        {
            switch(start - startTimeColumnsVerify.begin())
            {
            case(routeKeyStartTimeCol):
                qDebug() << "routeKeyStartTimeCol missing.\
                            Reverting to default precedence.";
                            break;

            case(startsPrevDayStartTimeCol):
                qDebug() << "startsPrevDayStartTimeCol missing.\
                            Reverting to default precedence.";
                            break;

            case(monStartTimeCol):
                qDebug() << "monStartTimeCol missing. \
                            Reverting to default precedence.";
                            break;

            case(tueStartTimeCol):
                qDebug() << "tueStartTimeCol missing.\
                            Reverting to default precedence.";
                            break;

            case(wedStartTimeCol):
                qDebug() << "wedStartTimeCol missing.\
                            Reverting to default precedence.";
                            break;

            case(thuStartTimeCol):
                qDebug() << "thuStartTimeCol missing.\
                            Reverting to default precedence.";
                            break;

            case(friStartTimeCol):
                qDebug() << "friStartTimeCol missing.\
                            Reverting to default precedence.";
                            break;

            case(satStartTimeCol):
                qDebug() << "satStartTimeCol missing.\
                            Reverting to default precedence.";
                            break;

            case(sunStartTimeCol):
                qDebug() << "sunStartTimeCol missing.\
                            Reverting to default precedence.";
                            break;
            }
        }
        start++;
    }
}

void MasterRoute::whatEmployeeColIsMissing(QVector<int> employeeColumnsVerify)
{
    //iterates through vector and provides the indexes of all duplicates.
    //This is then mapped to the enum type regarding what vector idx
    //applies to what column.
    int matchNotFound = -1;
    QVector<int>::iterator start = employeeColumnsVerify.begin();
    QVector<int>::iterator end = employeeColumnsVerify.end();

    while(end > start){
        start = std::find_if(start, end, [&matchNotFound](const int& j)\
        {return j == matchNotFound;});

        if(start != end)
        {
            switch(start - employeeColumnsVerify.begin())
            {
            case(employeeNameCol):
                qDebug() << "employeeNameCol missing.\
                            Reverting to default precedence.";
                            break;

            case(employeeNumCol):
                qDebug() << "employeeNumCol missing.\
                            Reverting to default precedence.";
                            break;
            }
        }
        start++;
    }
}

void MasterRoute::loadSettingsFromDatabase()
{
    QSqlDatabase settings;
    settings = settings.addDatabase("QSQLITE", "settings");
    settings.setDatabaseName(settingsPath_);
    settings.open();
    QSqlQuery query(settings);

    query.prepare("SELECT value FROM masterRouteSettings WHERE key = ...");
    query.exec();
    while(query.next())
    {
        if(query.value(0).toString() == "sheetsScope")
            sheetsScope = query.value(1).toString();

        if(query.value(0).toString() == "sheetAddressBase")
            sheetsAddressBase = query.value(2).toString();

        qDebug() << "Exec query?";
    }

    qDebug() << sheetsAddressBase << sheetsScope << "MasterRoute::loadSettingsFromDatabase";
    query.clear();
    settings.close();
    settings = QSqlDatabase();
    settings.removeDatabase("settings");
}

void MasterRoute::abort()
{
    qDebug() << "Network connection failed...";
    //oauthConn->abort(true);
    //networkProblem = true;
}

void MasterRoute::tryNetworkAgain()
{
    qDebug() << "Network connection failed...";
    //oauthConn->abort(false);
}

