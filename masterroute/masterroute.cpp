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
    QVector<Route> routes;
    Route route;
    QString dayOfWeekToQuery = dayOfWeek;
    bool foundDate;

    QVector<int> routeKeyFoundCol;
    QVector<int> driverFoundCol;
    QVector<int> equipmentFoundCol;
    QVector<int> miscFoundCol;

    QDateTime sheetDate;

    QJsonObject routeSheet = QJsonDocument::fromJson(queryRoutes(dayOfWeekToQuery)).object();
    QJsonArray routeArray = routeSheet["values"].toArray();
    QJsonArray routeTuple;

    if(routeStartTimes.empty())
        buildRouteStartTimes();

    if(employees.empty())
        buildEmployees();

    for(int row = 0; row < routeArray.size(); row++)
    {
        routeKeyFoundCol.clear();
        driverFoundCol.clear();
        equipmentFoundCol.clear();
        miscFoundCol.clear();

        routeTuple = routeArray.at(row).toArray();

        for(int col = 0; col < routeTuple.size(); col++)
        {
            switch(route.whatIsThis(routeTuple.at(col).toString()))
            {
            case routeEnum::matchSheetDate:
                foundDate = true;
                sheetDate = QDateTime::fromString(routeTuple.at(col).toString(), dateFormat);
                break;

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

        for(auto expectedRouteKeyCol: routeKeyFoundCol)
        {
            route = Route();

            int expectedDriverCol = expectedRouteKeyCol +  driverOffset;
            int expectedPowerUnitCol = expectedRouteKeyCol + powerUnitOffset;
            int expectedTrailerCol = expectedRouteKeyCol + trailerOffset;

            route.setField(routeTuple.at(expectedRouteKeyCol).toString(), routeEnum::key);

            if(driverFoundCol.contains(expectedDriverCol))
                route.setField(routeTuple.at(expectedDriverCol).toString(), routeEnum::driverName);

            if(equipmentFoundCol.contains(expectedPowerUnitCol))
                route.setField(routeTuple.at(expectedPowerUnitCol).toString(), routeEnum::truckNum);

            if(equipmentFoundCol.contains(expectedTrailerCol))
                route.setField(routeTuple.at(expectedTrailerCol).toString(), routeEnum::trailerNum);

            if(foundDate)
            {
                route.setField(sheetDate.toString(route.getDateFormat()), routeEnum::date);
            }

            routes.append(route);

        }
    }
    routes = applyStartTimeToRoutes(routes);
    routes = applyEmployeeNumsToRoutes(routes);

    for(auto t: routes)
        qDebug() << t.getKey() << t.getRouteDate().toUTC().toString() << t.getDriverName() << t.getDriverId();

    std::sort(routes.begin(), routes.end(), [](Route r1, Route r2) -> bool {return r1.getKey() < r2.getKey();});
    return routes;


}

//QVector<Route> MasterRoute::getRoutes()
//{
//    std::sort(routes.begin(), routes.end(), [](Route r1, Route r2) -> bool {return r1.getKey() < r2.getKey();});
//    return routes;
//}

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
        qDebug() << "Error with routeInfoPrecidence. There is a missing field. Check for route, driver, powerUnit, and trailer";
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

void MasterRoute::buildRouteStartTimes()
{
    QJsonObject startTimes = QJsonDocument::fromJson(queryRouteStartTimes()).object();
    QJsonArray startTimeArray = startTimes["values"].toArray();
    QJsonArray startTimeTuple;

    RouteStartTime routeStart;
    QString timeFormat  = "h:mm";
    int msecsInDay = 86400000;

    for(int row = 1; row < startTimeArray.size(); row++)
    {
        routeStart = RouteStartTime();
        startTimeTuple = startTimeArray.at(row).toArray();

        for(int col = 0; col < startTimeTuple.size(); col++)
        {
            if(mandatoryStartTimeColumns.at(routeKeyStartTimeCol) == col)
                routeStart.routeKey = startTimeTuple.at(col).toString();

            if(mandatoryStartTimeColumns.at(startsPrevDayStartTimeCol) == col)
            {
                routeStart.startsPrevDay[routeEnum::mon] = startTimeTuple.at(col).toString().contains("M");
                routeStart.startsPrevDay[routeEnum::tue] = startTimeTuple.at(col).toString().contains("T");
                routeStart.startsPrevDay[routeEnum::wed] = startTimeTuple.at(col).toString().contains("W");
                routeStart.startsPrevDay[routeEnum::thu] = startTimeTuple.at(col).toString().contains("R");
                routeStart.startsPrevDay[routeEnum::fri] = startTimeTuple.at(col).toString().contains("F");
                routeStart.startsPrevDay[routeEnum::sat] = startTimeTuple.at(col).toString().contains("S");
                routeStart.startsPrevDay[routeEnum::sun] = startTimeTuple.at(col).toString().contains("U");
            }

            if(mandatoryStartTimeColumns.at(monStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeEnum::mon])
                {
                    routeStart.monMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.monMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(tueStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay.at(routeEnum::tue))
                {
                    routeStart.tueMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.tueMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(wedStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeEnum::wed])
                {
                    routeStart.wedMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay()) - msecsInDay;
                }
                else
                {
                    routeStart.wedMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(thuStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeEnum::thu])
                {
                    routeStart.thuMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);

                }
                else
                {
                    routeStart.thuMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(friStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeEnum::fri])
                {
                    routeStart.friMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.friMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(satStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeEnum::sat])
                {
                    routeStart.satMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.satMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
            }

            if(mandatoryStartTimeColumns.at(sunStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeEnum::sun])
                {
                    routeStart.sunMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.sunMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
            }
        }
        routeStartTimes.append(routeStart);
    }
}

QVector<Route> MasterRoute::applyStartTimeToRoutes(QVector<Route> routes)
{
    QVector<Route>::iterator start = routes.begin();
    QVector<Route>::iterator end = routes.end();

    for(auto t: routeStartTimes)
    {
        QString toFind = t.routeKey;

        start = std::find_if(start, end, [&toFind](Route &j) {return j.getKey() == toFind;});

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
    QString employeeName;
    QString employeeNumber;
    QJsonObject employeeSheet = QJsonDocument::fromJson(queryEmployees()).object();
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
                routesIter->setField(employees[routesIter->getDriverName()], routeEnum::driverId);
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
//    if(!networkProblem){
        oauthConn->buildOAuth(sheetsScope, QString(sheetsAddressBase + dayOfWeekToQuery), sheetsCredFilePath);
        return oauthConn->get();
//    }
//    else
//        return QByteArray();
}

QByteArray MasterRoute::queryRouteStartTimes()
{
//    if(!networkProblem){
        oauthConn->buildOAuth(sheetsScope, sheetsStartTimeAddress, sheetsCredFilePath);
        return oauthConn->get();
//    }
//    else
//        return QByteArray();
}

QByteArray MasterRoute::queryEmployees()
{
//    if(!networkProblem){
        oauthConn->buildOAuth(sheetsScope, sheetsEmployeeAddress, sheetsCredFilePath);
        return oauthConn->get();
//    }
//    else
//        return QByteArray();
}

void MasterRoute::whatRouteColIsMissing()
{
}

void MasterRoute::whatRouteStartTimeColIsMissing(QVector<int> startTimeColumnsVerify)
{
    //iterates through vector and provides the indexes of all duplicates. This is then mapped to
    //the enum type regarding what vector idx applies to what column.
    int matchNotFound = -1;
    QVector<int>::iterator start = startTimeColumnsVerify.begin();
    QVector<int>::iterator end = startTimeColumnsVerify.end();

    while(end > start){
        start = std::find_if(start, end, [&matchNotFound](const int& j) {return j == matchNotFound;});
        if(start != end)
        {
            switch(start - startTimeColumnsVerify.begin())
            {
            case(routeKeyStartTimeCol):
                qDebug() << "routeKeyStartTimeCol missing. Reverting to default precedence.";
                break;

            case(startsPrevDayStartTimeCol):
                qDebug() << "startsPrevDayStartTimeCol missing. Reverting to default precedence.";
                break;

            case(monStartTimeCol):
                qDebug() << "monStartTimeCol missing. Reverting to default precedence.";
                break;

            case(tueStartTimeCol):
                qDebug() << "tueStartTimeCol missing. Reverting to default precedence.";
                break;

            case(wedStartTimeCol):
                qDebug() << "wedStartTimeCol missing. Reverting to default precedence.";
                break;

            case(thuStartTimeCol):
                qDebug() << "thuStartTimeCol missing. Reverting to default precedence.";
                break;

            case(friStartTimeCol):
                qDebug() << "friStartTimeCol missing. Reverting to default precedence.";
                break;

            case(satStartTimeCol):
                qDebug() << "satStartTimeCol missing. Reverting to default precedence.";
                break;

            case(sunStartTimeCol):
                qDebug() << "sunStartTimeCol missing. Reverting to default precedence.";
                break;
            }
        }
        start++;
    }
}

void MasterRoute::whatEmployeeColIsMissing(QVector<int> employeeColumnsVerify)
{
    //iterates through vector and provides the indexes of all duplicates. This is then mapped to
    //the enum type regarding what vector idx applies to what column.
    int matchNotFound = -1;
    QVector<int>::iterator start = employeeColumnsVerify.begin();
    QVector<int>::iterator end = employeeColumnsVerify.end();

    while(end > start){
        start = std::find_if(start, end, [&matchNotFound](const int& j) {return j == matchNotFound;});
        if(start != end)
        {
            switch(start - employeeColumnsVerify.begin())
            {
            case(employeeNameCol):
                qDebug() << "employeeNameCol missing. Reverting to default precedence.";
                break;

            case(employeeNumCol):
                qDebug() << "employeeNumCol missing. Reverting to default precedence.";
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

