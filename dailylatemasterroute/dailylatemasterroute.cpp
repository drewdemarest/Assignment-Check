#include "dailylatemasterroute.h"

DailyLateMasterRoute::DailyLateMasterRoute(QObject *parent) : QObject(parent)
{

}

DailyLateMasterRoute::~DailyLateMasterRoute()
{
    //oauthConn_->deleteLater();
    gm_->deleteLater();
}

void DailyLateMasterRoute::buildRoutes()
{
    routes_ = buildTodaysRoutes();
}

QVector<Route> DailyLateMasterRoute::buildTodaysRoutes()
{
    QVector<Route> routes;
    QJsonObject routeSheet = QJsonDocument::fromJson\
            (queryRoutes()).object();
    QJsonArray sheetValues = routeSheet["values"].toArray();

    routes = extractRoutesFromSheetValues(sheetValues);
    routes = applyEmployeeNumsToRoutes(routes);

    std::sort(routes.begin(), routes.end(), [](Route r1, Route r2) ->\
            bool {return r1.getKey() < r2.getKey();});

    for(auto t: routes)
        qDebug() << t.getKey() << t.getRouteDate().toUTC().toString()\
                 << t.getDriverName() << t.getDriverId() << "rte dbg";

    return routes;
}

QVector<Route> DailyLateMasterRoute::getRoutes()
{
    return routes_;
}

QVector<RouteDifference> DailyLateMasterRoute::compareRoutesToGreenmile()
{
    //Important to set the dates every time this function is called.
    dailyLateRoutesBegin_.setDate(QDate::currentDate());
    dailyLateRoutesEnd_ = QDateTime::currentDateTime();

    QStringList dlmrsRouteKeys;
    QStringList routeDifferenceKeys;

    QVector<Route> dlmrsRoutes = buildTodaysRoutes();
    QVector<Route> greenmileRoutes =\
        gm_->getRoutesForTimeInterval(dailyLateRoutesBegin_, dailyLateRoutesEnd_);

    QVector<RouteDifference> routeDifferences =\
            RouteDifference::findDifferences(dlmrsRoutes, greenmileRoutes);


    for(auto routeDifference: routeDifferences)
    {
        routeDifferenceKeys.append(routeDifference.routeKey);
    }

    for(auto dlmrsRoute : dlmrsRoutes)
    {
        dlmrsRouteKeys.append(dlmrsRoute.getKey());
    }

    //If differences contains a key NOT in the daily late sheet, remove that
    //route difference from the vector.
    //Dispatch is ONLY concerned with routes in their sheet unlike
    //the night routes which know a little about everything.
    for(auto it = routeDifferenceKeys.begin(); it != routeDifferenceKeys.end();)
    {
        if(dlmrsRouteKeys.indexOf(*it) == -1)
        {
            int idxRemove = it - routeDifferenceKeys.begin();
            routeDifferences.remove(idxRemove);
            it = routeDifferenceKeys.erase(it);
        }
        else
            ++it;
    }

    //If there's no mismatch between driver and truck, remove the route.
    //While it is feasilbe that dispatch would upload a route with a null
    //driver and truck... it's highly unlikely that this would ever happen.
    //99.99% of the time anything wrong is actually a misassignment.
    for(auto it = routeDifferences.begin(); it != routeDifferences.end();)
    {
        if(!it->driverMismatch && !it->truckMismatch)
            it = routeDifferences.erase(it);
        else
            ++it;
    }

    return routeDifferences;
}

void DailyLateMasterRoute::setRouteInfoPrecedence
(QStringList &routeInfoPrecedence)
{
    const QVector<int> routeIndexVector
    {
        routeInfoPrecedence.indexOf("route"),
        routeInfoPrecedence.indexOf("driver"),
        routeInfoPrecedence.indexOf("timeIn"),
        routeInfoPrecedence.indexOf("timeOut"),
        routeInfoPrecedence.indexOf("powerUnit")
    };

    if(!routeIndexVector.contains(-1))
    {
        const int routeIndex        = routeIndexVector.at(0);
        const int driverIndex       = routeIndexVector.at(1);
        const int timeInIndex       = routeIndexVector.at(2);
        const int timeOutIndex      = routeIndexVector.at(3);
        const int powerUnitIndex    = routeIndexVector.at(4);

        routeOffset_     = routeIndex - routeIndex;
        driverOffset_    = driverIndex - routeIndex;
        timeInOffset_    = timeInIndex - routeIndex;
        timeOutOffset_   = timeOutIndex - routeIndex;
        powerUnitOffset_ = powerUnitIndex - routeIndex;
    }
    else
    {
        whatRouteFieldIsMissing(routeIndexVector);
    }
}


void DailyLateMasterRoute::setEmployeeInfoPrecedence
(QStringList &employeeInfoPrecedence)
{
    QVector<int> employeeColumnsVerify =
    {
        employeeInfoPrecedence.indexOf("employeeName"),
        employeeInfoPrecedence.indexOf("employeeNum"),
    };

    if(!employeeColumnsVerify.contains(-1))
    {
        mandatoryEmployeeColumns_ = employeeColumnsVerify;
    }
    else
    {
        whatEmployeeColIsMissing(employeeColumnsVerify);
    }
}


QVector<Route> DailyLateMasterRoute::extractRoutesFromSheetValues
(const QJsonArray &sheetValues)
{
    Route route;
    QVector<Route> routes;
    QVector<int> routeKeyFoundCol;
    QVector<int> driverFoundCol;
    QVector<int> equipmentFoundCol;
    QVector<int> miscFoundCol;
    QVector<int> timeFoundCol;

    QJsonArray row;

    for(int rowCount = 0; rowCount < sheetValues.size(); rowCount++)
    {
        route = Route();
        row = sheetValues.at(rowCount).toArray();

        routeKeyFoundCol.clear();
        driverFoundCol.clear();
        equipmentFoundCol.clear();
        miscFoundCol.clear();
        timeFoundCol.clear();

        for(int colCount = 0; colCount < row.size(); colCount++)
        {
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

            case routeEnum::matchTime:
                timeFoundCol.append(colCount);
                break;
            }
        }

        for(auto expectedRouteKeyCol: routeKeyFoundCol)
        {
            route = Route();

            int expectedDriverCol = expectedRouteKeyCol +  driverOffset_;
            int expectedPowerUnitCol = expectedRouteKeyCol + powerUnitOffset_;
            int expectedTimeOutCol = expectedRouteKeyCol + timeOutOffset_;

            route.setField(row.at(expectedRouteKeyCol)\
                           .toString(), routeEnum::key);

            if(driverFoundCol.contains(expectedDriverCol))
            {
                route.setField(row.at(expectedDriverCol).toString(),\
                               routeEnum::driverName);
            }

            if(equipmentFoundCol.contains(expectedPowerUnitCol))
            {
                route.setField(row.at(expectedPowerUnitCol).toString(),\
                               routeEnum::truckNum);
            }

            if(timeFoundCol.contains(expectedTimeOutCol))
            {
                route.setField(row.at(expectedTimeOutCol).toString(),\
                               routeEnum::lateRouteOutTime);
            }

            //-----------------------------------------------------------------
            // Fix me! I am hideous!
            //-----------------------------------------------------------------
//            if(!route.getKey().isEmpty() &&
//               !route.getDriverName().isEmpty() &&
//               !route.getTruckNumber().isEmpty())
//            {
            if(!route.getKey().isEmpty())
            {
                if(route.getRouteDate().isNull() ||
                   !route.getRouteDate().isValid())
                {
                    route.setField(QTime::currentTime().toString("hh:mm"),
                                   routeEnum::lateRouteOutTime);
                }
                routes.append(route);
            }
            //-----------------------------------------------------------------
        }
    }
    return routes;
}

QMap<QString, QString> DailyLateMasterRoute::buildEmployees()
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
            if(mandatoryEmployeeColumns_.at(employeeNameCol) == colCount)
            {
                employeeName = row.at(colCount).toString();
            }
            if(mandatoryEmployeeColumns_.at(employeeNumCol) == colCount)
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

QVector<Route> DailyLateMasterRoute::applyEmployeeNumsToRoutes\
(QVector<Route> routes)
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

QByteArray DailyLateMasterRoute::queryRoutes()
{
//    oauthConn_->buildOAuth(sheetsScope_,
//                           QString(sheetsRouteAddress_),
//                           sheetsCredFilePath_);

//    return oauthConn_->get();
    return QByteArray();
}

QByteArray DailyLateMasterRoute::queryEmployees()
{
//    oauthConn_->buildOAuth(sheetsScope_,
//                           sheetsEmployeeAddress_,
//                           sheetsCredFilePath_);

//    return oauthConn_->get();
    return QByteArray();
}

void DailyLateMasterRoute::whatRouteFieldIsMissing
(QVector<int> routeFieldVerify)
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
            }
        }
        start++;
    }
}


void DailyLateMasterRoute::whatEmployeeColIsMissing
(QVector<int> employeeColumnsVerify)
{
    //iterates through vector and provides the indexes of all duplicates.
    //This is then mapped to the enum type regarding what vector
    //idx applies to what column.

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
                qDebug() << "employeeNameCol missing. "
                            "Reverting to default precedence.";
                break;

            case(employeeNumCol):
                qDebug() << "employeeNumCol missing. "
                            "Reverting to default precedence.";
                break;
            }
        }
        start++;
    }
}

void DailyLateMasterRoute::abort()
{
    qDebug() << "Network connection failed...";
    //oauthConn_->abort(true);
}

void DailyLateMasterRoute::tryNetworkAgain()
{
    qDebug() << "Network connection failed... trying again.";
    //oauthConn_->abort(false);
}
