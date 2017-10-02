#include "greenmile.h"

Greenmile::Greenmile(QObject *parent) : QObject(parent)
{
    //getRoutesForTimeInterval(QDateTime::currentDateTime().addSecs(-18000).toUTC(), QDateTime::currentDateTime().toUTC());
}

Greenmile::~Greenmile()
{

}

QVector<RouteDifference> Greenmile::compareRoutesToGreenmileRoutes(const QVector<Route> &masterRouteRoutes)
{
    //QVector<Route> masterRouteRoutesCopy = masterRouteRoutes;
    makeTimeIntervalForQuery(masterRouteRoutes);

    if(minQueryDateTime.isNull() || maxQueryDateTime.isNull())
    {
        qDebug() << "Greenmile query datetimes are null, returning empty route vector";
         QVector<RouteDifference> emptyRouteDiffVector;
         return emptyRouteDiffVector;
    }

    QByteArray response = queryGreenmile(minQueryDateTime.toUTC(),maxQueryDateTime.toUTC());
    buildRoutesFromGreenmileResponse(response);

    routeDifferences = compareRoutes(routes, masterRouteRoutes);

//    for(auto t: routeDifferences)
//    {
//          t.printDebug();
//    }
    return routeDifferences;
}

void Greenmile::makeTimeIntervalForQuery(const QVector<Route> &r)
{
    minQueryDateTime = QDateTime();
    maxQueryDateTime = QDateTime();
    QVector<Route> rte = r;
    if(rte.isEmpty())
    {
        qDebug() << "Greenmile: cannot create query interval from empty vector";
        return;
    }
    std::sort(rte.begin(), rte.end(), [](Route r1, Route r2) ->\
            bool {return r1.getRouteDate() < r2.getRouteDate();});

    minQueryDateTime = rte.first().getRouteDate().addMSecs(-timeIntervalPaddingMsec);
    maxQueryDateTime = rte.last().getRouteDate().addMSecs(timeIntervalPaddingMsec);
    qDebug() << minQueryDateTime.toString(Qt::ISODate);
    qDebug() << maxQueryDateTime.toString(Qt::ISODate);
}

QByteArray Greenmile::queryGreenmile(const QDateTime &begin, const QDateTime &end)
{
    loadHeadersFromJson();
    QJsonObject postBodyJson;
    QJsonArray criteriaChain;

    QJsonObject evaluationStatement;
    QJsonArray logicalConditions;

    QJsonObject queryFromDateTime;
    queryFromDateTime["attr"] = QString("baseLineDeparture");
    queryFromDateTime["gte"] = begin.toString(Qt::ISODate);

    QJsonObject queryToDateTime;
    queryToDateTime["attr"] = QString("baseLineDeparture");
    queryToDateTime["lte"] = end.toString(Qt::ISODate);

    logicalConditions.append(queryFromDateTime);
    logicalConditions.append(queryToDateTime);

    evaluationStatement["and"] = logicalConditions;

    criteriaChain.append(evaluationStatement);

    postBodyJson["criteriaChain"] = criteriaChain;

    //qDebug() << QJsonDocument(postBodyJson).toJson(QJsonDocument::Compact);

    body = QJsonDocument(postBodyJson).toJson(QJsonDocument::Compact);
    return greenmileConnection->postRequest(headers, gmRouteIntervalAddress, body);
}

void Greenmile::buildRoutesFromGreenmileResponse(const QByteArray &gmResponse)
{
    routes.clear();
    Route route;
    QJsonArray routeArray = QJsonDocument::fromJson(gmResponse).array();
    qDebug() << routeArray.size();

    for(auto t: routeArray)
    {
        route = Route();

        QJsonObject tRoute = t.toObject();
        //qDebug() << tRoute["key"].toString();
        route.setField(tRoute["key"].toString(), routeEnum::key);
        QJsonArray tEquipmentAssignments =
                tRoute["equipmentAssignments"].toArray();
        QJsonArray tDriverAssignments =
                tRoute["driverAssignments"].toArray();

        for(auto e: tEquipmentAssignments)
        {
            QJsonObject tEquipmentAssignment = e.toObject();
            QJsonObject  tEquipment = tEquipmentAssignment["equipment"].toObject();
            route.setField(tEquipment["key"].toString(), routeEnum::truckNum);
        }

        for(auto d: tDriverAssignments)
        {
            QJsonObject tDriverAssignment = d.toObject();
            QJsonObject tDriver = tDriverAssignment["driver"].toObject();
            route.setField(tDriver["key"].toString(), routeEnum::driverId);
            route.setField(tDriver["name"].toString(), routeEnum::driverName);
        }
        routes.append(route);
    }
}

QVector<RouteDifference> Greenmile::compareRoutes(const QVector<Route> &greenmileRoutes, const QVector<Route> &masterRouteRoutes)
{
    RouteDifference  routeDiff;
    QVector<RouteDifference> routeDiffs;

    for(auto greenmileRoute: greenmileRoutes)
    {
        routeDiff = RouteDifference();
        routeDiff.routeKey = greenmileRoute.getKey();
        routeDiff.greenmileTruck = greenmileRoute.getTruckNumber();
        routeDiff.greenmileDriverID = greenmileRoute.getDriverId();
        routeDiff.greenmileDriverName = greenmileRoute.getDriverName();
        auto masterRouteRouteStart = masterRouteRoutes.begin();
        auto masterRouteRouteEnd = masterRouteRoutes.end();

        QString routeKeyToFind = greenmileRoute.getKey();
        masterRouteRouteStart = std::find_if(masterRouteRouteStart, masterRouteRouteEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});
        if(masterRouteRouteStart == masterRouteRouteEnd)
        {
            routeDiff.routeExistsInMasterRoute = false;
            routeDiff.hasDiscrepencies = true;
            routeDiff.driverMismatch = true;
            routeDiff.truckMismatch = true;
        }

        while(masterRouteRouteStart < masterRouteRouteEnd)
        {
            masterRouteRouteStart = std::find_if(masterRouteRouteStart, masterRouteRouteEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});

            if(masterRouteRouteStart != masterRouteRouteEnd)
            {
                if(greenmileRoute.getDriverId() != masterRouteRouteStart->getDriverId())
                {
                    routeDiff.greenmileTruck = greenmileRoute.getTruckNumber();
                    routeDiff.masterRouteTruck = masterRouteRouteStart->getTruckNumber();
                    routeDiff.greenmileDriverID = greenmileRoute.getDriverId();
                    routeDiff.greenmileDriverName = greenmileRoute.getDriverName();
                    routeDiff.masterRouteDriverName = masterRouteRouteStart->getDriverName();
                    routeDiff.masterRouteDriverID = masterRouteRouteStart->getDriverId();
                    routeDiff.hasDiscrepencies = true;
                    routeDiff.driverMismatch = true;
                }
                if(greenmileRoute.getTruckNumber() != masterRouteRouteStart->getTruckNumber())
                {
                    routeDiff.greenmileTruck = greenmileRoute.getTruckNumber();
                    routeDiff.masterRouteTruck = masterRouteRouteStart->getTruckNumber();
                    routeDiff.greenmileDriverID = greenmileRoute.getDriverId();
                    routeDiff.greenmileDriverName = greenmileRoute.getDriverName();
                    routeDiff.masterRouteDriverName = masterRouteRouteStart->getDriverName();
                    routeDiff.masterRouteDriverID = masterRouteRouteStart->getDriverId();
                    routeDiff.hasDiscrepencies = true;
                    routeDiff.truckMismatch = true;
                }
            }
            masterRouteRouteStart++;
        }
        if(routeDiff.hasDiscrepencies)
        {
            if(routeDiff.greenmileDriverID == routeDiff.masterRouteDriverID)
                routeDiff.driverMismatch = false;

            if(routeDiff.greenmileTruck == routeDiff.masterRouteTruck)
                routeDiff.truckMismatch = false;

            routeDiffs.append(routeDiff);
        }
    }

    for(auto masterRouteRoute: masterRouteRoutes)
    {
        routeDiff = RouteDifference();
        routeDiff.routeKey = masterRouteRoute.getKey();
        routeDiff.masterRouteTruck = masterRouteRoute.getTruckNumber();
        routeDiff.masterRouteDriverID = masterRouteRoute.getDriverId();
        routeDiff.masterRouteDriverName = masterRouteRoute.getDriverName();
        auto greenmileRoutesStart = greenmileRoutes.begin();
        auto greenmileRoutesEnd = greenmileRoutes.end();

        QString routeKeyToFind = masterRouteRoute.getKey();
        greenmileRoutesStart = std::find_if(greenmileRoutesStart, greenmileRoutesEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});
        if(greenmileRoutesStart == greenmileRoutesEnd)
        {
            routeDiff.routeExistsInGreenmile = false;
            routeDiff.hasDiscrepencies = true;
            routeDiff.driverMismatch = true;
            routeDiff.truckMismatch = true;
        }

        while(greenmileRoutesStart < greenmileRoutesEnd)
        {
            greenmileRoutesStart = std::find_if(greenmileRoutesStart, greenmileRoutesEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});

            if(greenmileRoutesStart != greenmileRoutesEnd)
            {
                if(masterRouteRoute.getDriverId() != greenmileRoutesStart->getDriverId())
                {
                    routeDiff.masterRouteDriverID = masterRouteRoute.getDriverId();
                    routeDiff.masterRouteDriverName = masterRouteRoute.getDriverName();
                    routeDiff.greenmileDriverID = greenmileRoutesStart->getDriverId();
                    routeDiff.greenmileDriverName  = greenmileRoutesStart->getDriverName();
                    routeDiff.masterRouteTruck = masterRouteRoute.getTruckNumber();
                    routeDiff.greenmileTruck = greenmileRoutesStart->getTruckNumber();
                    routeDiff.hasDiscrepencies = true;
                    routeDiff.driverMismatch = true;
                }
                if(masterRouteRoute.getTruckNumber() != greenmileRoutesStart->getTruckNumber())
                {
                    routeDiff.masterRouteDriverID = masterRouteRoute.getDriverId();
                    routeDiff.masterRouteDriverName = masterRouteRoute.getDriverName();
                    routeDiff.greenmileDriverID = greenmileRoutesStart->getDriverId();
                    routeDiff.greenmileDriverName  = greenmileRoutesStart->getDriverName();
                    routeDiff.masterRouteTruck = masterRouteRoute.getTruckNumber();
                    routeDiff.greenmileTruck = greenmileRoutesStart->getTruckNumber();
                    routeDiff.hasDiscrepencies = true;
                    routeDiff.truckMismatch = true;
                }
            }
            greenmileRoutesStart++;
        }

        if(routeDiff.hasDiscrepencies)
        {
            if(routeDiff.greenmileDriverID == routeDiff.masterRouteDriverID)
                routeDiff.driverMismatch = false;

            if(routeDiff.greenmileTruck == routeDiff.masterRouteTruck)
                routeDiff.truckMismatch = false;

            routeDiffs.append(routeDiff);
        }

    }

    std::sort(routeDiffs.begin(), routeDiffs.end(), [](RouteDifference rd1, RouteDifference rd2) -> bool {return rd1.routeKey < rd2.routeKey;});
    auto last = std::unique(routeDiffs.begin(), routeDiffs.end());
    routeDiffs.erase(last, routeDiffs.end());

    return routeDiffs;
}

void Greenmile::loadHeadersFromJson()
{
    headers.clear();

    QByteArray headersRaw;
    QFile jsonHeaderFile(headersFilePath);
    if(!jsonHeaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while(!jsonHeaderFile.atEnd())
    {
        headersRaw.append(jsonHeaderFile.readLine());
    }

    QJsonObject headersFromJson = QJsonObject(QJsonDocument::fromJson(headersRaw).object());

    for(auto t: headersFromJson["requestHeaders"].toArray())
    {
        headers << t.toString();
    }
    qDebug() << "headers" << headers;
}

void Greenmile::saveHeadersToJson()
{

}
