#include "masterroute.h"

MasterRoute::MasterRoute(QObject *parent) : QObject(parent)
{
    buildRouteStartTimes();
}

void MasterRoute::buildRoutes()
{
    Route route;
    QString dayOfWeekToQuery = "test";
    bool foundDate;

    QJsonObject routeSheet = QJsonDocument::fromJson(queryRoutes(dayOfWeekToQuery)).object();
    QJsonArray routeArray = routeSheet["values"].toArray();
    QJsonArray routeTuple;

    QVector<int> routeKeyFoundCol;
    QVector<int> driverFoundCol;
    QVector<int> equipmentFoundCol;
    QVector<int> miscFoundCol;

    QDateTime sheetDate;


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
            qDebug() << route.getRouteDate().toString() << route.getKey() <<  route.getDriverName() << route.getTruckNumber() << route.getTrailerNumber();
        }
    }
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
        startTimeColumns = startTimeColumnsVerify;
    }
    else
    {
        whatRouteStartTimeColIsMissing(startTimeColumnsVerify);
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
        startTimeTuple = startTimeArray.at(row).toArray();

        for(int col = 0; col < startTimeTuple.size(); col++)
        {
            if(startTimeColumns.at(routeKeyStartTimeCol) == col)
                routeStart.routeKey = startTimeTuple.at(col).toString();

            if(startTimeColumns.at(startsPrevDayStartTimeCol) == col)
            {
                routeStart.startsPrevDay[routeStart.mon] = startTimeTuple.at(col).toString().contains("M");
                routeStart.startsPrevDay[routeStart.tue] = startTimeTuple.at(col).toString().contains("T");
                routeStart.startsPrevDay[routeStart.wed] = startTimeTuple.at(col).toString().contains("W");
                routeStart.startsPrevDay[routeStart.thu] = startTimeTuple.at(col).toString().contains("R");
                routeStart.startsPrevDay[routeStart.fri] = startTimeTuple.at(col).toString().contains("F");
                routeStart.startsPrevDay[routeStart.sat] = startTimeTuple.at(col).toString().contains("S");
                routeStart.startsPrevDay[routeStart.sun] = startTimeTuple.at(col).toString().contains("U");
            }

            if(startTimeColumns.at(monStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeStart.mon])
                {
                    routeStart.monMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.monMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
                qDebug() << startTimeTuple.at(col).toString() << routeStart.monMidnightOffsetmSec;
            }

            if(startTimeColumns.at(tueStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeStart.tue])
                {
                    routeStart.tueMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.tueMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
                qDebug() << startTimeTuple.at(col).toString() << routeStart.tueMidnightOffsetmSec;
            }

            if(startTimeColumns.at(wedStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeStart.wed])
                {
                    routeStart.wedMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay()) - msecsInDay;
                }
                else
                {
                    routeStart.wedMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
                qDebug() << startTimeTuple.at(col).toString() << routeStart.wedMidnightOffsetmSec;
            }

            if(startTimeColumns.at(thuStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeStart.thu])
                {
                    routeStart.thuMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);

                }
                else
                {
                    routeStart.thuMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
                qDebug() <<startTimeTuple.at(col).toString() << routeStart.monMidnightOffsetmSec;
            }

            if(startTimeColumns.at(friStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeStart.fri])
                {
                    routeStart.friMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.friMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
                qDebug() <<startTimeTuple.at(col).toString() << routeStart.friMidnightOffsetmSec;
            }

            if(startTimeColumns.at(satStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeStart.sat])
                {
                    routeStart.satMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.satMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
                qDebug() << startTimeTuple.at(col).toString() << routeStart.satMidnightOffsetmSec;
            }

            if(startTimeColumns.at(sunStartTimeCol) == col)
            {
                if(routeStart.startsPrevDay[routeStart.sun])
                {
                    routeStart.sunMidnightOffsetmSec = (QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay() - msecsInDay);
                }
                else
                {
                    routeStart.sunMidnightOffsetmSec = QTime::fromString(startTimeTuple.at(col).toString(), timeFormat).msecsSinceStartOfDay();
                }
                qDebug() << startTimeTuple.at(col).toString() << routeStart.sunMidnightOffsetmSec;
            }
        }

    }
}

QByteArray MasterRoute::queryRoutes(QString &dayOfWeekToQuery)
{
    oauthConn->buildOAuth(sheetsScope, QString(sheetsAddressBase + dayOfWeekToQuery), sheetsCredFilePath);
    return oauthConn->get();
}

QByteArray MasterRoute::queryRouteStartTimes()
{
    oauthConn->buildOAuth(sheetsScope, sheetsStartTimeAddress, sheetsCredFilePath);
    return oauthConn->get();
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

