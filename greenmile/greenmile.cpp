#include "greenmile.h"

Greenmile::Greenmile(QObject *parent) : QObject(parent)
{
    //getRoutesForTimeInterval(QDateTime::currentDateTime().addSecs(-18000).toUTC(), QDateTime::currentDateTime().toUTC());
}

Greenmile::~Greenmile()
{

}

QVector<RouteDifference> Greenmile::compareRoutesToGreenmileRoutes(const QVector<Route> &otherRoutes)
{
    //QVector<Route> otherRoutesCopy = otherRoutes;
    makeTimeIntervalForQuery(otherRoutes);

    RouteDifference routeDiff;

    if(minQueryDateTime.isNull() || maxQueryDateTime.isNull())
    {
        qDebug() << "Greenmile query datetimes are null, returning empty route vector";
         QVector<RouteDifference> emptyRouteDiffVector;
         return emptyRouteDiffVector;
    }

    QByteArray response = queryGreenmile(minQueryDateTime.toUTC(),maxQueryDateTime.toUTC());
    buildRoutesFromGreenmileResponse(response);


    for(auto t: routes)
    {
        routeDiff = RouteDifference();
        routeDiff.routeKey = t.getKey();
        routeDiff.greenmileTruck = t.getTruckNumber();
        routeDiff.greenmileDriverID = t.getDriverId();
        auto otherStart = otherRoutes.begin();
        auto otherEnd = otherRoutes.end();

        QString routeKeyToFind = t.getKey();
        otherStart = std::find_if(otherStart, otherEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});
        if(otherStart == otherEnd)
        {
            //qDebug() << "Master route does not contain" << t.getKey();
            routeDiff.routeExistsInMasterRoute = false;
            routeDiff.hasDiscrepencies = true;
        }

        while(otherStart < otherEnd)
        {
            otherStart = std::find_if(otherStart, otherEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});

            if(otherStart != otherEnd)
            {
                if(t.getDriverId() != otherStart->getDriverId())
                {
                    //qDebug() << "ID mismatch tkey" << t.getKey() << "other key" << otherStart->getKey() << "gm id" << t.getDriverId() << "other id" << otherStart->getDriverId();
                    routeDiff.greenmileTruck = t.getTruckNumber();
                    routeDiff.masterRouteTruck = otherStart->getTruckNumber();
                    routeDiff.greenmileDriverID = t.getDriverId();
                    routeDiff.masterRouteDriverID = otherStart->getDriverId();
                    routeDiff.hasDiscrepencies = true;
                }
                if(t.getTruckNumber() != otherStart->getTruckNumber())
                {
                    //qDebug() << "truck mismatch tkey" << t.getKey() << "other key" << otherStart->getKey() << "gm id" << t.getTruckNumber() << "other id" << otherStart->getTruckNumber();
                    routeDiff.greenmileTruck = t.getTruckNumber();
                    routeDiff.masterRouteTruck = otherStart->getTruckNumber();
                    routeDiff.greenmileDriverID = t.getDriverId();
                    routeDiff.masterRouteDriverID = otherStart->getDriverId();
                    routeDiff.hasDiscrepencies = true;
                }
            }
            otherStart++;
        }
        if(routeDiff.hasDiscrepencies)
        {
            routeDifferences.append(routeDiff);
        }
    }

    for(auto t: otherRoutes)
    {
        routeDiff = RouteDifference();
        routeDiff.routeKey = t.getKey();
        routeDiff.masterRouteTruck = t.getTruckNumber();
        routeDiff.masterRouteDriverID = t.getDriverId();
        auto otherStart = routes.begin();
        auto otherEnd = routes.end();

        QString routeKeyToFind = t.getKey();
        otherStart = std::find_if(otherStart, otherEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});
        if(otherStart == otherEnd)
        {
            //qDebug() << "GM does not contain" << t.getKey();
            routeDiff.routeExistsInGreenmile = false;
            routeDiff.hasDiscrepencies = true;
        }

        while(otherStart < otherEnd)
        {
            otherStart = std::find_if(otherStart, otherEnd, [&routeKeyToFind](Route u) {return routeKeyToFind == u.getKey();});

            if(otherStart != otherEnd)
            {
                if(t.getDriverId() != otherStart->getDriverId())
                {
                    //() << "ID mismatch mrs" << t.getKey() << "gm key" << otherStart->getKey() << "mrs id" << t.getDriverId() << "gm id" << otherStart->getDriverId();
                    routeDiff.masterRouteDriverID = t.getDriverId();
                    routeDiff.greenmileDriverID = otherStart->getDriverId();
                    routeDiff.masterRouteTruck = t.getTruckNumber();
                    routeDiff.greenmileTruck = otherStart->getTruckNumber();
                    routeDiff.hasDiscrepencies = true;
                }
                if(t.getTruckNumber() != otherStart->getTruckNumber())
                {
                    //qDebug() << "truck mismatch mrs" << t.getKey() << "gm key" << otherStart->getKey() << "mrs id" << t.getTruckNumber() << "gm id" << otherStart->getTruckNumber();
                    routeDiff.masterRouteDriverID = t.getDriverId();
                    routeDiff.greenmileDriverID = otherStart->getDriverId();
                    routeDiff.masterRouteTruck = t.getTruckNumber();
                    routeDiff.greenmileTruck = otherStart->getTruckNumber();
                    routeDiff.hasDiscrepencies = true;
                }
            }
            otherStart++;
        }

        if(routeDiff.hasDiscrepencies)
        {
            routeDifferences.append(routeDiff);
        }

    }

    std::sort(routeDifferences.begin(), routeDifferences.end(), [](RouteDifference rd1, RouteDifference rd2) -> bool {return rd1.routeKey < rd2.routeKey;});

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
