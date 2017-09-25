#include "greenmile.h"

Greenmile::Greenmile(QObject *parent) : QObject(parent)
{
    //getRoutesForTimeInterval(QDateTime::currentDateTime().addSecs(-18000).toUTC(), QDateTime::currentDateTime().toUTC());
}

Greenmile::~Greenmile()
{

}

QVector<Route> Greenmile::compareRoutesToGreenmileRoutes(const QVector<Route> &otherRoutes)
{
    makeTimeIntervalForQuery(otherRoutes);

    if(minQueryDateTime.isNull() || maxQueryDateTime.isNull())
    {
        qDebug() << "Greenmile query datetimes are null, returning empty route vector";
         QVector<Route> emptyRouteVector;
         return emptyRouteVector;
    }

    QByteArray response = queryGreenmile(minQueryDateTime.toUTC(),maxQueryDateTime.toUTC());
    buildRoutesFromGreenmileResponse(response);
    for(auto t: routes)
    {
        qDebug() << t.getTruckNumber() << t.getKey() << t.getDriverId() << t.getDriverName();
    }
    return routes;
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
