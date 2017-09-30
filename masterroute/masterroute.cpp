#include "masterroute.h"

MasterRoute::MasterRoute(QObject *parent) : QObject(parent)
{
}

MasterRoute::~MasterRoute()
{
    oauthConn->deleteLater();
}

void MasterRoute::buildAllRoutes()
{ 
    mondayRoutes = buildRoutes(mondaySheetTitle);
    tuesdayRoutes = buildRoutes(tuesdaySheetTitle);
    wednesdayRoutes = buildRoutes(wednesdaySheetTitle);
    thursdayRoutes = buildRoutes(thursdaySheetTitle);
    fridayRoutes = buildRoutes(fridaySheetTitle);
    saturdayRoutes = buildRoutes(saturdaySheetTitle);
    sundayRoutes = buildRoutes(sundaySheetTitle);
}

void MasterRoute::buildMondayRoutes()
{
    mondayRoutes = buildRoutes(mondaySheetTitle);
}

void MasterRoute::buildTuesdayRoutes()
{
    tuesdayRoutes = buildRoutes(tuesdaySheetTitle);
}

void MasterRoute::buildWednesdayRoutes()
{
    wednesdayRoutes = buildRoutes(wednesdaySheetTitle);
}

void MasterRoute::buildThursdayRoutes()
{
    thursdayRoutes = buildRoutes(thursdaySheetTitle);
}

void MasterRoute::buildFridayRoutes()
{
    fridayRoutes = buildRoutes(fridaySheetTitle);
}

void MasterRoute::buildSaturdayRoutes()
{
    saturdayRoutes = buildRoutes(saturdaySheetTitle);
}

void MasterRoute::buildSundayRoutes()
{
    sundayRoutes = buildRoutes(sundaySheetTitle);
}

QVector<Route> MasterRoute::getMondayRoutes()
{
    return mondayRoutes;
}

QVector<Route> MasterRoute::getTuesdayRoutes()
{
    return tuesdayRoutes;
}

QVector<Route> MasterRoute::getWednesdayRoutes()
{
    return wednesdayRoutes;
}

QVector<Route> MasterRoute::getThursdayRoutes()
{
    return thursdayRoutes;
}

QVector<Route> MasterRoute::getFridayRoutes()
{
    return fridayRoutes;
}

QVector<Route> MasterRoute::getSaturdayRoutes()
{
    return saturdayRoutes;
}

QVector<Route> MasterRoute::getSundayRoutes()
{
    return sundayRoutes;
}

QVector<Route> MasterRoute::buildRoutes(QString dayOfWeek)
{
    QDateTime sheetDate;
    QVector<Route> routes;

    QJsonObject routeSheet = QJsonDocument::fromJson\
            (queryRoutes(dayOfWeek)).object();

    QJsonArray routeArray = routeSheet["values"].toArray();
    QJsonArray routeTuple;

    buildRouteStartTimes();
    buildEmployees();
    sheetDate = extractSheetDate(routeArray);
    for(int row = 0; row < routeArray.size(); row++)
    {
        routeTuple = routeArray.at(row).toArray();
        routes.append(extractRoutesFromSheet(routeTuple));
    }

    routes = applyStartTimeToRoutes(routes);
    routes = applyEmployeeNumsToRoutes(routes);  

    std::sort(routes.begin(), routes.end(), [](Route r1, Route r2) ->\
            bool {return r1.getKey() < r2.getKey();});

    for(auto t: routes)
        qDebug() << t.getKey() << t.getRouteDate().toUTC().toString()\
                 << t.getDriverName() << t.getDriverId();

    return routes;
}

QVector<Route> MasterRoute::extractRoutesFromSheet(const QJsonArray &sheetValues)
{
    Route route;
    QVector<Route> routes;
    QVector<int> routeKeyFoundCol;
    QVector<int> driverFoundCol;
    QVector<int> equipmentFoundCol;
    QVector<int> miscFoundCol;

    for(int col = 0; col < row.size(); col++)
    {
        //-----------------------------------------------------------------
        // Asks the route what this cell is. If enough of these columns
        // are populated with data then a potential route has been
        // located.
        //-----------------------------------------------------------------
        switch(route.whatIsThis(row.at(col).toString()))
        {
        case routeEnum::matchDriverName:
            driverFoundCol.append(col);
            break;

        case routeEnum::matchEquipment:
            equipmentFoundCol.append(col);
            break;

        case routeEnum::matchMisc:
            miscFoundCol.append(col);
            break;

        case routeEnum::matchRouteKey:
            routeKeyFoundCol.append(col);
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

    return routes;
}

void MasterRoute::setRouteInfoPrecedence(QStringList &routeInfoPrecedence)
{
    const QVector<int> routeIndexVector
    {
        routeInfoPrecedence.indexOf("route"),
        routeInfoPrecedence.indexOf("driver"),
        routeInfoPrecedence.indexOf("powerUnit"),
        routeInfoPrecedence.indexOf("trailer"),
    };

    if(!routeIndexVector.contains(-1))
    {
        const int routeIndex        = routeIndexVector.at(0);
        const int driverIndex       = routeIndexVector.at(1);
        const int powerUnitIndex    = routeIndexVector.at(2);
        const int trailerIndex      = routeIndexVector.at(3);

        routeOffset     = routeIndex - routeIndex;
        driverOffset    = driverIndex - routeIndex;
        powerUnitOffset = powerUnitIndex - routeIndex;
        trailerOffset   = trailerIndex - routeIndex;
    }
    else
    {
        qDebug() << "Error with routeInfoPrecidence.\
                     There is a missing field.\
                     Check for route, driver,\
                     powerUnit, and trailer";
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

void MasterRoute::buildRouteStartTimes()
{
    routeStartTimes.clear();
    QJsonObject startTimes =
            QJsonDocument::fromJson(queryRouteStartTimes()).object();
    QJsonArray startTimeArray = startTimes["values"].toArray();
    QJsonArray startTimeTuple;

    for(int row = 1; row < startTimeArray.size(); row++)
    {
       startTimeTuple = startTimeArray.at(row).toArray();
       routeStartTimes.append(buildRouteStartTimeFromRow(startTimeTuple));
    }
}

RouteStartTime MasterRoute::buildRouteStartTimeFromRow(const QJsonArray &row)
{
    RouteStartTime routeStart;

    QString timeFormat  = "h:mm";
    int msecsInDay = 86400000;

    for(int col = 0; col < row.size(); col++)
    {
        if(mandatoryStartTimeColumns.at(routeKeyStartTimeCol) == col)
            routeStart.routeKey = row.at(col).toString();

        if(mandatoryStartTimeColumns.at(startsPrevDayStartTimeCol) == col)
        {
            routeStart.startsPrevDay[routeEnum::mon] =
                    row.at(col).toString().contains("M");
            routeStart.startsPrevDay[routeEnum::tue] =
                    row.at(col).toString().contains("T");
            routeStart.startsPrevDay[routeEnum::wed] =
                    row.at(col).toString().contains("W");
            routeStart.startsPrevDay[routeEnum::thu] =
                    row.at(col).toString().contains("R");
            routeStart.startsPrevDay[routeEnum::fri] =
                    row.at(col).toString().contains("F");
            routeStart.startsPrevDay[routeEnum::sat] =
                    row.at(col).toString().contains("S");
            routeStart.startsPrevDay[routeEnum::sun] =
                    row.at(col).toString().contains("U");
        }

        if(mandatoryStartTimeColumns.at(monStartTimeCol) == col)
        {
            if(routeStart.startsPrevDay[routeEnum::mon])
            {
                routeStart.monMidnightOffsetmSec =
                        (QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);
            }
            else
            {
                routeStart.monMidnightOffsetmSec =
                        QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay();
            }
        }

        if(mandatoryStartTimeColumns.at(tueStartTimeCol) == col)
        {
            if(routeStart.startsPrevDay.at(routeEnum::tue))
            {
                routeStart.tueMidnightOffsetmSec =
                        (QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);
            }
            else
            {
                routeStart.tueMidnightOffsetmSec =
                        QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay();
            }
        }

        if(mandatoryStartTimeColumns.at(wedStartTimeCol) == col)
        {
            if(routeStart.startsPrevDay[routeEnum::wed])
            {
                routeStart.wedMidnightOffsetmSec =
                        (QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay()) - msecsInDay;
            }
            else
            {
                routeStart.wedMidnightOffsetmSec =
                        QTime::fromString(row.at(col).toString(),
                            timeFormat).msecsSinceStartOfDay();
            }
        }

        if(mandatoryStartTimeColumns.at(thuStartTimeCol) == col)
        {
            if(routeStart.startsPrevDay[routeEnum::thu])
            {
                routeStart.thuMidnightOffsetmSec =
                        (QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);

            }
            else
            {
                routeStart.thuMidnightOffsetmSec =
                        QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay();
            }
        }

        if(mandatoryStartTimeColumns.at(friStartTimeCol) == col)
        {
            if(routeStart.startsPrevDay[routeEnum::fri])
            {
                routeStart.friMidnightOffsetmSec =
                        (QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);
            }
            else
            {
                routeStart.friMidnightOffsetmSec =
                        QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay();
            }
        }

        if(mandatoryStartTimeColumns.at(satStartTimeCol) == col)
        {
            if(routeStart.startsPrevDay[routeEnum::sat])
            {
                routeStart.satMidnightOffsetmSec =
                        (QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);
            }
            else
            {
                routeStart.satMidnightOffsetmSec =
                        QTime::fromString(row.at(col).toString(),\
                              timeFormat).msecsSinceStartOfDay();
            }
        }

        if(mandatoryStartTimeColumns.at(sunStartTimeCol) == col)
        {
            if(routeStart.startsPrevDay[routeEnum::sun])
            {
                routeStart.sunMidnightOffsetmSec =
                        (QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay() - msecsInDay);
            }
            else
            {
                routeStart.sunMidnightOffsetmSec =
                        QTime::fromString(row.at(col).toString(),\
                            timeFormat).msecsSinceStartOfDay();
            }
        }
    }

    return routeStart;
}

QVector<Route> MasterRoute::applyStartTimeToRoutes(QVector<Route> routes)
{
    QVector<Route>::iterator start = routes.begin();
    QVector<Route>::iterator end = routes.end();

    for(auto t: routeStartTimes)
    {
        QString toFind = t.routeKey;

        start = std::find_if(start, end, [&toFind](Route &j)\
            {return j.getKey() == toFind;});

        if(start != end)
        {
            start->applyFullStartTimeOffset(t);
        }
        start = routes.begin();
        end = routes.end();
    }
    return routes;
}

void MasterRoute::buildEmployees()
{
    employees.clear();
    QString employeeName;
    QString employeeNumber;
    QJsonObject employeeSheet =
            QJsonDocument::fromJson(queryEmployees()).object();
    QJsonArray employeeArray = employeeSheet["values"].toArray();
    QJsonArray employeeTuple;

    for(int row = 1; row < employeeArray.size(); row++)
    {
        employeeTuple = employeeArray.at(row).toArray();

        for(int col = 0; col < employeeTuple.size(); col++)
        {
            if(mandatoryEmployeeColumns.at(employeeNameCol) == col)
            {
                employeeName = employeeTuple.at(col).toString();
            }
            if(mandatoryEmployeeColumns.at(employeeNumCol) == col)
            {
                employeeNumber = employeeTuple.at(col).toString();
            }

        }
        employees[employeeName] = employeeNumber;
        employeeName = QString();
        employeeNumber = QString();
    }
}

QVector<Route> MasterRoute::applyEmployeeNumsToRoutes(QVector<Route> routes)
{
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
    oauthConn->buildOAuth(sheetsScope,\
                          QString(sheetsAddressBase + dayOfWeekToQuery),\
                          sheetsCredFilePath);

    return oauthConn->get();
}

QByteArray MasterRoute::queryRouteStartTimes()
{
    oauthConn->buildOAuth(sheetsScope,
                          sheetsStartTimeAddress,
                          sheetsCredFilePath);
    return oauthConn->get();
}

QByteArray MasterRoute::queryEmployees()
{
    oauthConn->buildOAuth(sheetsScope,
                          sheetsEmployeeAddress,
                          sheetsCredFilePath);
    return oauthConn->get();
}

void MasterRoute::whatRouteColIsMissing()
{
    //Hmm
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

void MasterRoute::abort()
{
    qDebug() << "Network connection failed...";
    oauthConn->abort(true);
    //networkProblem = true;
}

void MasterRoute::tryNetworkAgain()
{
    qDebug() << "Network connection failed...";
    oauthConn->abort(false);
}

