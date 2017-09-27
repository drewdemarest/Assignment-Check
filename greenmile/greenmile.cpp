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

    RouteDifference routeDiff;

    if(minQueryDateTime.isNull() || maxQueryDateTime.isNull())
    {
        qDebug() << "Greenmile query datetimes are null, returning empty route vector";
         QVector<RouteDifference> emptyRouteDiffVector;
         return emptyRouteDiffVector;
    }

    QByteArray response = queryGreenmile(minQueryDateTime.toUTC(),maxQueryDateTime.toUTC());
    buildRoutesFromGreenmileResponse(response);

    routeDifferences = compareRoutes(routes, masterRouteRoutes);


    for(auto t: routeDifferences)
    {
          t.printDebug();
    }
    return routeDifferences;
}

void Greenmile::makeTimeIntervalForQuery(const QVector<Route> &r)
{
    QVector<Route> rte = r;
    if(rte.isEmpty())
    {
        qDebug() << "Greenmile: cannot create query interval from empty vector";
        return;
    }
    std::sort(rte.begin(), rte.end(), [](Route r1, Route r2) -> bool {return r1.getRouteDate() < r2.getRouteDate();});
    minQueryDateTime = rte.first().getRouteDate();
    maxQueryDateTime = rte.last().getRouteDate();
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
        //qDebug() << route.getTruckNumber() << route.getKey() << route.getDriverId() << route.getDriverName();
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
        auto greenmileRoutesStart = masterRouteRoutes.begin();
        auto greenmileRoutesEnd = masterRouteRoutes.end();

        QString routeKeyToFind = greenmileRoute.getKey();
        greenmileRoutesStart = std::find_if(greenmileRoutesStart, greenmileRoutesEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});
        if(greenmileRoutesStart == greenmileRoutesEnd)
        {
            //qDebug() << "Master route does not contain" << greenmileRoute.getKey();
            routeDiff.routeExistsInMasterRoute = false;
            routeDiff.hasDiscrepencies = true;
        }

        while(greenmileRoutesStart < greenmileRoutesEnd)
        {
            greenmileRoutesStart = std::find_if(greenmileRoutesStart, greenmileRoutesEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});

            if(greenmileRoutesStart != greenmileRoutesEnd)
            {
                if(greenmileRoute.getDriverId() != greenmileRoutesStart->getDriverId())
                {
                    //qDebug() << "ID mismatch tkey" << greenmileRoute.getKey() << "other key" << greenmileRoutesStart->getKey() << "gm id" << greenmileRoute.getDriverId() << "other id" << greenmileRoutesStart->getDriverId();
                    routeDiff.greenmileTruck = greenmileRoute.getTruckNumber();
                    routeDiff.masterRouteTruck = greenmileRoutesStart->getTruckNumber();
                    routeDiff.greenmileDriverID = greenmileRoute.getDriverId();
                    routeDiff.masterRouteDriverID = greenmileRoutesStart->getDriverId();
                    routeDiff.hasDiscrepencies = true;
                    routeDiff.driverMismatch = true;
                }
                if(greenmileRoute.getTruckNumber() != greenmileRoutesStart->getTruckNumber())
                {
                    //qDebug() << "truck mismatch tkey" << greenmileRoute.getKey() << "other key" << greenmileRoutesStart->getKey() << "gm id" << greenmileRoute.getTruckNumber() << "other id" << greenmileRoutesStart->getTruckNumber();
                    routeDiff.greenmileTruck = greenmileRoute.getTruckNumber();
                    routeDiff.masterRouteTruck = greenmileRoutesStart->getTruckNumber();
                    routeDiff.greenmileDriverID = greenmileRoute.getDriverId();
                    routeDiff.masterRouteDriverID = greenmileRoutesStart->getDriverId();
                    routeDiff.hasDiscrepencies = true;
                    routeDiff.truckMismatch = true;
                }
            }
            greenmileRoutesStart++;
        }
        if(routeDiff.hasDiscrepencies)
        {
            routeDiffs.append(routeDiff);
        }
    }

    for(auto masterRouteRoute: masterRouteRoutes)
    {
        routeDiff = RouteDifference();
        routeDiff.routeKey = masterRouteRoute.getKey();
        routeDiff.masterRouteTruck = masterRouteRoute.getTruckNumber();
        routeDiff.masterRouteDriverID = masterRouteRoute.getDriverId();
        auto masterRouteRoutesStart = routes.begin();
        auto masterRouteRoutesEnd = routes.end();

        QString routeKeyToFind = masterRouteRoute.getKey();
        masterRouteRoutesStart = std::find_if(masterRouteRoutesStart, masterRouteRoutesEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});
        if(masterRouteRoutesStart == masterRouteRoutesEnd)
        {
            //qDebug() << "GM does not contain" << masterRouteRoute.getKey();
            routeDiff.routeExistsInGreenmile = false;
            routeDiff.hasDiscrepencies = true;
        }

        while(masterRouteRoutesStart < masterRouteRoutesEnd)
        {
            masterRouteRoutesStart = std::find_if(masterRouteRoutesStart, masterRouteRoutesEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});

            if(masterRouteRoutesStart != masterRouteRoutesEnd)
            {
                if(masterRouteRoute.getDriverId() != masterRouteRoutesStart->getDriverId())
                {
                    //() << "ID mismatch mrs" << masterRouteRoute.getKey() << "gm key" << masterRouteRoutesStart->getKey() << "mrs id" << masterRouteRoute.getDriverId() << "gm id" << masterRouteRoutesStart->getDriverId();
                    routeDiff.masterRouteDriverID = masterRouteRoute.getDriverId();
                    routeDiff.greenmileDriverID = masterRouteRoutesStart->getDriverId();
                    routeDiff.masterRouteTruck = masterRouteRoute.getTruckNumber();
                    routeDiff.greenmileTruck = masterRouteRoutesStart->getTruckNumber();
                    routeDiff.hasDiscrepencies = true;
                    routeDiff.driverMismatch = true;
                }
                if(masterRouteRoute.getTruckNumber() != masterRouteRoutesStart->getTruckNumber())
                {
                    //qDebug() << "truck mismatch mrs" << masterRouteRoute.getKey() << "gm key" << masterRouteRoutesStart->getKey() << "mrs id" << masterRouteRoute.getTruckNumber() << "gm id" << masterRouteRoutesStart->getTruckNumber();
                    routeDiff.masterRouteDriverID = masterRouteRoute.getDriverId();
                    routeDiff.greenmileDriverID = masterRouteRoutesStart->getDriverId();
                    routeDiff.masterRouteTruck = masterRouteRoute.getTruckNumber();
                    routeDiff.greenmileTruck = masterRouteRoutesStart->getTruckNumber();
                    routeDiff.hasDiscrepencies = true;
                    routeDiff.truckMismatch = true;
                }
            }
            masterRouteRoutesStart++;
        }

        if(routeDiff.hasDiscrepencies)
        {
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
