#include "masterroute.h"

MasterRoute::MasterRoute(QObject *parent) : QObject(parent)
{

}

void MasterRoute::buildRoutes(const QJsonObject &qjo)
{

}

void MasterRoute::buildRoutes(const QByteArray &qba)
{
    Route route;
    int matchIdx = -1;
    bool foundDate = false;

    QJsonObject routeSheet = QJsonDocument::fromJson(qba).object();
    QJsonArray routeArray = routeSheet["values"].toArray();
    QJsonArray routeTuple;

    QVector<int> routeKeyFound;
    QVector<int> equipmentFound;
    QVector<int> driverFound;
    QVector<int> miscFound;;

    for(int i = 0; i < routeArray.size(); i++)
    {
        routeKeyFound.clear();
        equipmentFound.clear();
        driverFound.clear();
        miscFound.clear();

        routeTuple = routeArray.at(i).toArray();

        for(int j = 0; j < routeTuple.size(); j++)
        {
            matchIdx = -1;
            matchIdx = routeRegExp.indexIn(routeTuple.at(j).toString());
            if(matchIdx != -1)
            {
                //qDebug() << "Row" << i << "Col" << j << "Route:" << routeRegExp.cap(matchIdx);
                routeKeyFound.append(j);
                continue;
            }

            matchIdx = -1;
            matchIdx = driverRegExp.indexIn(routeTuple.at(j).toString());
            if(matchIdx != -1)
            {
                //qDebug() << "Row" << i << "Col" << j << "Driver:" << driverRegExp.cap(matchIdx);
                driverFound.append(j);
                continue;
            }

            matchIdx = -1;
            matchIdx = equipmentRegExp.indexIn(routeTuple.at(j).toString());
            if(matchIdx != -1)
            {
                //qDebug() << "Row" << i << "Col" << j << "Equipment:" << equipmentRegExp.cap(matchIdx);
                equipmentFound.append(j);
                continue;
            }

            miscFound.append(j);
        }

        for(auto expectedRouteKeyCol: routeKeyFound)
        {
            route = Route();
            //qDebug() << routeTuple.at(0).toString();
            //qDebug() << routeInfoPrecedence;
            int expectedDriverCol = expectedRouteKeyCol +  driverOffset;
            int expectedPowerUnitCol = expectedRouteKeyCol + powerUnitOffset;
            int expectedTrailerCol = expectedRouteKeyCol + trailerOffset;
            int expectedNotesCol = expectedRouteKeyCol + notesOffset;

            //qDebug() << expectedRouteKeyCol << expectedDriverCol << expectedPowerUnitCol << expectedTrailerCol << expectedNotesCol;

            route.routeKey = routeTuple.at(expectedRouteKeyCol).toString();

            if(driverFound.contains(expectedDriverCol))
                route.driverName = routeTuple.at(expectedDriverCol).toString();

            if(equipmentFound.contains(expectedPowerUnitCol))
               route.truckNumber = routeTuple.at(expectedPowerUnitCol).toString();

            if(equipmentFound.contains(expectedTrailerCol))
                route.trailerNumber = routeTuple.at(expectedTrailerCol).toString();

            if(miscFound.contains(expectedNotesCol))
                route.notes = routeTuple.at(expectedNotesCol).toString();

            qDebug() << route.routeKey <<  route.driverName << route.truckNumber << route.trailerNumber << route.notes;
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
        routeInfoPrecedence.indexOf("notes")
    };

    if(!routeIndexVector.contains(-1))
    {
        const int routeIndex =        routeIndexVector.at(0);
        const int driverIndex =       routeIndexVector.at(1);
        const int powerUnitIndex =    routeIndexVector.at(2);
        const int trailerIndex =      routeIndexVector.at(3);
        const int notesIndex =        routeIndexVector.at(4);

        //everything builds relative to the route. Route is 0, all other fields
        //are calculated based on the difference between them and route
        routeOffset = routeIndex - routeIndex;
        driverOffset = driverIndex - routeIndex;
        powerUnitOffset = powerUnitIndex - routeIndex;
        trailerOffset = trailerIndex - routeIndex;
        notesOffset = notesIndex - routeIndex;

        qDebug() << routeOffset << driverOffset << powerUnitOffset << trailerOffset << notesOffset;

    }
    else
    {
        qDebug() << "Error with routeInfoPrecidence. There is a missing field";
    }

}
