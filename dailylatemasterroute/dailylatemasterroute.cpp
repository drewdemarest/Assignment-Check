#include "dailylatemasterroute.h"

DailyLateMasterRoute::DailyLateMasterRoute(QObject *parent) : QObject(parent)
{

}

DailyLateMasterRoute::~DailyLateMasterRoute()
{
    oauthConn->deleteLater();
}

void DailyLateMasterRoute::buildRoutes()
{
    //QVector<Route> routes;
    routes.clear();
    Route route;
    bool foundDate;

    QVector<int> routeKeyFoundCol;
    QVector<int> driverFoundCol;
    QVector<int> equipmentFoundCol;
    QVector<int> miscFoundCol;
    QVector<int> timeFoundCol;

    QDateTime sheetDate;

    QJsonObject routeSheet = QJsonDocument::fromJson(queryRoutes()).object();
    QJsonArray routeArray = routeSheet["values"].toArray();
    QJsonArray routeTuple;

    buildEmployees();

    for(int row = 0; row < routeArray.size(); row++)
    {
        routeKeyFoundCol.clear();
        driverFoundCol.clear();
        equipmentFoundCol.clear();
        miscFoundCol.clear();
        timeFoundCol.clear();

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

            case routeEnum::matchTime:
                timeFoundCol.append(col);
                break;
            }
        }

        for(auto expectedRouteKeyCol: routeKeyFoundCol)
        {
            route = Route();

            int expectedDriverCol = expectedRouteKeyCol +  driverOffset;
            int expectedPowerUnitCol = expectedRouteKeyCol + powerUnitOffset;
            //int expectedTimeInCol = expectedRouteKeyCol + timeInOffset;
            int expectedTimeOutCol = expectedRouteKeyCol + timeOutOffset;

            route.setField(routeTuple.at(expectedRouteKeyCol).toString(), routeEnum::key);

            if(driverFoundCol.contains(expectedDriverCol))
                route.setField(routeTuple.at(expectedDriverCol).toString(), routeEnum::driverName);

            if(equipmentFoundCol.contains(expectedPowerUnitCol))
                route.setField(routeTuple.at(expectedPowerUnitCol).toString(), routeEnum::truckNum);

            if(equipmentFoundCol.contains(expectedTimeOutCol))
                route.setField(routeTuple.at(expectedTimeOutCol).toString(), routeEnum::lateRouteOutTime);

            if(!route.getKey().isEmpty(), !route.getDriverName().isEmpty() && !route.getTruckNumber().isEmpty())
            {
                if(route.getRouteDate().isNull())
                {
                    route.setField(QTime::currentTime().toString("h:mm"), routeEnum::lateRouteOutTime);
                }
                routes.append(route);
            }
        }
    }

    //Needs reimplement to use what's in the route tuple.
    //routes = applyStartTimeToRoutes(routes);
    routes = applyEmployeeNumsToRoutes(routes);

    for(auto t: routes)
        qDebug() << t.getKey() << t.getRouteDate().toUTC().toString() << t.getDriverName() << t.getDriverId();

    std::sort(routes.begin(), routes.end(), [](Route r1, Route r2) -> bool {return r1.getKey() < r2.getKey();});
    //return routes;
}

QVector<Route> DailyLateMasterRoute::getRoutes()
{
    return routes;
}

void DailyLateMasterRoute::setRouteInfoPrecedence(QStringList &routeInfoPrecedence)
{
    const QVector<int> routeIndexVector
    {
        routeInfoPrecedence.indexOf("route"),
                routeInfoPrecedence.indexOf("driver"),
                routeInfoPrecedence.indexOf("timeIn"),
                routeInfoPrecedence.indexOf("timeOut"),
                routeInfoPrecedence.indexOf("prowerUnit")
    };

    if(!routeIndexVector.contains(-1))
    {
        const int routeIndex        = routeIndexVector.at(0);
        const int driverIndex       = routeIndexVector.at(1);
        const int timeInIndex       = routeIndexVector.at(2);
        const int timeOutIndex      = routeIndexVector.at(3);
        const int powerUnitIndex    = routeIndexVector.at(4);

        routeOffset     = routeIndex - routeIndex;
        driverOffset    = driverIndex - routeIndex;
        timeInOffset    = timeInIndex - routeIndex;
        timeOutOffset   = timeOutIndex - routeIndex;
        powerUnitOffset = powerUnitIndex - routeIndex;
    }
    else
    {
        qDebug() << "Error with routeInfoPrecidence. There is a missing field. Check for route, driver, powerUnit, and trailer";
    }
}


void DailyLateMasterRoute::setEmployeeInfoPrecedence(QStringList &employeeInfoPrecedence)
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

void DailyLateMasterRoute::buildEmployees()
{
    employees.clear();
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

QVector<Route> DailyLateMasterRoute::applyEmployeeNumsToRoutes(QVector<Route> routes)
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

QByteArray DailyLateMasterRoute::queryRoutes()
{
    oauthConn->buildOAuth(sheetsScope, QString(sheetsRouteAddress), sheetsCredFilePath);
    return oauthConn->get();
}

QByteArray DailyLateMasterRoute::queryEmployees()
{
    oauthConn->buildOAuth(sheetsScope, sheetsEmployeeAddress, sheetsCredFilePath);
    return oauthConn->get();
}

void DailyLateMasterRoute::whatRouteColIsMissing()
{
    //Hmm
}

void DailyLateMasterRoute::whatEmployeeColIsMissing(QVector<int> employeeColumnsVerify)
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

void DailyLateMasterRoute::abort()
{
    qDebug() << "Network connection failed...";
    oauthConn->abort(true);
    //networkProblem = true;
}

void DailyLateMasterRoute::tryNetworkAgain()
{
    qDebug() << "Network connection failed...";
    oauthConn->abort(false);
}
