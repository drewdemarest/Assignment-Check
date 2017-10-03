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
    RouteDifference  routeDiff;
    QByteArray response;

    makeTimeIntervalForQuery(masterRouteRoutes);

    if(minQueryDateTime_.isNull() || maxQueryDateTime_.isNull())
    {
        qDebug() << "Greenmile query datetimes are null,"
                    " returning empty route vector";

         QVector<RouteDifference> emptyRouteDiffVector;
         return emptyRouteDiffVector;
    }

    response = queryGreenmile(minQueryDateTime_.toUTC(),
                              maxQueryDateTime_.toUTC());

    buildRoutesFromGreenmileResponse(response);

    //routeDiff.findDifferences(masterRouteRoutes, routes_);
    return routeDiff.findDifferences(masterRouteRoutes, routes_);
}

QVector<RouteDifference> Greenmile::compareDLMRSToGreenmileRoutes
(const QVector<Route> &dlmrsRoutes)
{
    routes_.clear();
    routeDifferences_.clear();
    RouteDifference routeDiff;
    QByteArray response;
    QStringList routeDiffKeys;
    QStringList dlmrsKeys;

    minDLMRSDateTime_ = QDateTime::currentDateTime();
    minDLMRSDateTime_.setTime(QTime::fromString("430", "h:mm"));

    maxDLMRSDateTime_ = QDateTime::currentDateTime();

    if(minDLMRSDateTime_ > maxDLMRSDateTime_)
    {
        qDebug() << "DLMRS min time greater than DLMRS max"
                    " time...  returning empty route vector"
                    "GM CLASS COMPARE FUNCTION";

         QVector<RouteDifference> emptyRouteDiffVector;
         return emptyRouteDiffVector;
    }

    response = queryGreenmile(minDLMRSDateTime_.toUTC(),
                              maxDLMRSDateTime_.toUTC());

    buildRoutesFromGreenmileResponse(response);

    routeDifferences_ = routeDiff.findDifferences(dlmrsRoutes, routes_);

    for(auto difference: routeDifferences_)
    {
        routeDiffKeys.append(difference.routeKey);
    }

    for(auto dlmrs : dlmrsRoutes)
    {
        dlmrsKeys.append(dlmrs.getKey());
    }

    qDebug() << "Before" << routeDiffKeys;

    for(auto it = routeDiffKeys.begin(); it != routeDiffKeys.end();)
    {
        if(dlmrsKeys.indexOf(*it) == -1)
        {
            int idxRemove = it - routeDiffKeys.begin();
            routeDifferences_.remove(idxRemove);

            it = routeDiffKeys.erase(it);
        }
        else
            ++it;
    }

    for(auto it = routeDifferences_.begin(); it != routeDifferences_.end();)
    {
        if(!it->driverMismatch && !it->truckMismatch)
        {
            it = routeDifferences_.erase(it);
        }
        else
            ++it;
    }

    qDebug() << "After" << routeDiffKeys;

    return routeDifferences_;

}

void Greenmile::makeTimeIntervalForQuery(const QVector<Route> &r)
{
    minQueryDateTime_ = QDateTime();
    maxQueryDateTime_ = QDateTime();
    QVector<Route> rte = r;
    if(rte.isEmpty())
    {
        qDebug() << "Greenmile: cannot create query interval from empty vector";
        return;
    }
    std::sort(rte.begin(), rte.end(), [](Route r1, Route r2) ->\
            bool {return r1.getRouteDate() < r2.getRouteDate();});

    minQueryDateTime_ = rte.first().getRouteDate().addMSecs(-timeIntervalPaddingMsec_);
    maxQueryDateTime_ = rte.last().getRouteDate().addMSecs(timeIntervalPaddingMsec_);
    qDebug() << minQueryDateTime_.toString(Qt::ISODate);
    qDebug() << maxQueryDateTime_.toString(Qt::ISODate);
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

    body_ = QJsonDocument(postBodyJson).toJson(QJsonDocument::Compact);
    return greenmileConnection_->postRequest(headers_, gmRouteIntervalAddress_, body_);
}

void Greenmile::buildRoutesFromGreenmileResponse(const QByteArray &gmResponse)
{
    routes_.clear();
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
        routes_.append(route);
    }
}

void Greenmile::loadHeadersFromJson()
{
    headers_.clear();

    QByteArray headersRaw;
    QFile jsonHeaderFile(headersFilePath_);
    if(!jsonHeaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while(!jsonHeaderFile.atEnd())
    {
        headersRaw.append(jsonHeaderFile.readLine());
    }

    QJsonObject headersFromJson = QJsonObject(QJsonDocument::fromJson(headersRaw).object());

    for(auto t: headersFromJson["requestHeaders"].toArray())
    {
        headers_ << t.toString();
    }
    qDebug() << "headers" << headers_;
}

void Greenmile::saveHeadersToJson()
{

}
