#include "masterroute.h"

MasterRoute::MasterRoute(QObject *parent) : QObject(parent)
{
    qDebug() << queryRouteStartTimes();
}

void MasterRoute::buildRoutes()
{
    QDate sheetDate;
    QString dateFormat = "d-MMM-yyyy";

    Route route;
    int matchIdx = -1;
    bool foundDate = false;
    bool foundMatch = false;

    QString dayOfWeekToQuery = "test";

    QJsonObject routeSheet = QJsonDocument::fromJson(queryRoutes(dayOfWeekToQuery)).object();
    QJsonArray routeArray = routeSheet["values"].toArray();
    QJsonArray routeTuple;

    QVector<int> routeKeyFound;
    QVector<int> equipmentFound;
    QVector<int> driverFound;
    QVector<int> miscFound;;

    for(int row = 0; row < routeArray.size(); row++)
    {
        routeKeyFound.clear();
        equipmentFound.clear();
        driverFound.clear();
        miscFound.clear();

        routeTuple = routeArray.at(row).toArray();

        for(int col = 0; col < routeTuple.size(); col++)
        {
            foundMatch = false;

            for(int k = 0; k < regExpVector.size(); k++)
            {
                if(foundMatch)
                    break;

                if(foundDate && k == matchSheetDate)
                    continue;

                //qDebug() << k;
                matchIdx = -1;
                matchIdx = regExpVector.at(k)->indexIn(routeTuple.at(col).toString());
                if(matchIdx != -1)
                {
                    switch(k)
                    {
                    case matchSheetDate:
                        sheetDate = QDate::fromString(mrsSheetDateRegExp.cap(matchIdx), dateFormat);
                        foundDate = true;
                        foundMatch = true;
                        break;

                    case matchRoute:
                        routeKeyFound.append(col);
                        foundMatch = true;
                        break;

                    case matchDriver:
                        driverFound.append(col);
                        foundMatch = true;
                        break;

                    case matchEquipment:
                        equipmentFound.append(col);
                        foundMatch = true;
                        break;
                    }
                }
            }
            //It would just be foundmatch but... some poeple stick white space in the sheets
            if(!foundMatch && !routeTuple.at(col).toString().trimmed().isEmpty()){
                miscFound.append(col);
            }
        }


        for(auto expectedRouteKeyCol: routeKeyFound)
        {
            route = Route();

            int expectedDriverCol = expectedRouteKeyCol +  driverOffset;
            int expectedPowerUnitCol = expectedRouteKeyCol + powerUnitOffset;
            int expectedTrailerCol = expectedRouteKeyCol + trailerOffset;

            route.routeKey = routeTuple.at(expectedRouteKeyCol).toString();

            if(driverFound.contains(expectedDriverCol))
                route.driverName = routeTuple.at(expectedDriverCol).toString();

            if(equipmentFound.contains(expectedPowerUnitCol))
               route.truckNumber = routeTuple.at(expectedPowerUnitCol).toString();

            if(equipmentFound.contains(expectedTrailerCol))
                route.trailerNumber = routeTuple.at(expectedTrailerCol).toString();

            if(foundDate)
            {
                route.baselineDeparture = QDateTime(sheetDate);
            }

            routes.append(route);
            qDebug() << route.baselineDeparture.toString() << route.routeKey <<  route.driverName << route.truckNumber << route.trailerNumber;
        }

//        qDebug() << "row" << i;
//        qDebug() << "keys:" << routeKeyFound;
//        qDebug() << "drivers:" << driverFound;
//        qDebug() << "equipment:" << equipmentFound;
//        qDebug() << "misc" << miscFound << miscFound.size();
//        qDebug() << " ";

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
        //routeInfoPrecedence.indexOf("notes")
    };


    if(!routeIndexVector.contains(-1))
    {
        const int routeIndex =        routeIndexVector.at(0);
        const int driverIndex =       routeIndexVector.at(1);
        const int powerUnitIndex =    routeIndexVector.at(2);
        const int trailerIndex =      routeIndexVector.at(3);

        //everything builds relative to the route. Route is 0, all other fields
        //are calculated based on the difference between them and route
        routeOffset = routeIndex - routeIndex;
        driverOffset = driverIndex - routeIndex;
        powerUnitOffset = powerUnitIndex - routeIndex;
        trailerOffset = trailerIndex - routeIndex;

        //notesOffset = notesIndex - routeIndex;

        //qDebug() << routeOffset << driverOffset << powerUnitOffset << trailerOffset << notesOffset;

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

    int matchNotFound = -1;
    QVector<int>::iterator start = startTimeColumnsVerify.begin();
    QVector<int>::iterator end = startTimeColumnsVerify.end();

    if(!startTimeColumnsVerify.contains(-1))
    {
        startTimeColumns = startTimeColumnsVerify;
    }
    else
    {
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


}

void MasterRoute::buildRouteStartTimes()
{
    QJsonObject startTimes = QJsonDocument::fromJson(queryRouteStartTimes()).object();
    QJsonArray startTimeArray = startTimes["values"].toArray();
    QJsonArray startTimeTuple;

    QVector<int> routeKeyFound;


//    for(int i = 0; i < startTimeArray.size(); i++)
//    {
//        routeKeyFound.clear();

//        startTimeTuple = startTimeArray.at(i).toArray();

//        for(int j = 0; j < startTimeTuple.size(); j++)
//        {
//            matchIdx = -1;
//            matchIdx = routeRegExp.indexIn(routeTuple.at(j).toString());
//            if(matchIdx != -1)
//            {
//                //qDebug() << "Row" << i << "Col" << j << "Route:" << routeRegExp.cap(matchIdx);
//                routeKeyFound.append(j);
//                continue;
//            }
//        }


//    }

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
