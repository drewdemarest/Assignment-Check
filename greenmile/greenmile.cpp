#include "greenmile.h"

Greenmile::Greenmile(QObject *parent) : QObject(parent)
{
}

Greenmile::~Greenmile()
{
    greenmileConnection_->deleteLater();
}


QVector<Route> Greenmile::getRoutesForTimeInterval\
(const QDateTime &minQueryDateTime, const QDateTime &maxQueryDateTime)
{
    QByteArray response;
    QVector<Route> greenmileRoutes;

    if(timeIntervalHasError(minQueryDateTime, maxQueryDateTime))
         return greenmileRoutes;

    response = queryGreenmileBaselineStartTime(minQueryDateTime.toUTC(),
                                               maxQueryDateTime.toUTC());

    return buildRoutesFromGreenmileResponse(response);
}

QVector<RouteDifference> Greenmile::compareRoutesToGMRoutesRouteDate(const QVector<Route> &otherRoutes)
{
    QByteArray response;
    QVector<Route> greenmileRoutes;

    makeDateIntervalForQuery(otherRoutes);

//    if(!routeDate.isValid() || routeDate.isNull())
//         return greenmileRoutes;

    response = queryGreenmileRouteDate(minQueryDate_, maxQueryDate_);

    greenmileRoutes = buildRoutesFromGreenmileResponse(response);

    return RouteDifference::findDifferences(otherRoutes, greenmileRoutes);
}

QVector<RouteDifference> Greenmile::compareRoutesToGMRoutesBaselineStart\
(const QVector<Route> &otherRoutes)
{
    QVector<Route> greenmileRoutes;
    QByteArray response;

    makeDateTimeIntervalForQuery(otherRoutes);

    if(timeIntervalHasError(minQueryDateTime_, maxQueryDateTime_))
         return QVector<RouteDifference>();

    response = queryGreenmileBaselineStartTime(minQueryDateTime_.toUTC(),
                                               maxQueryDateTime_.toUTC());

    greenmileRoutes = buildRoutesFromGreenmileResponse(response);

    return RouteDifference::findDifferences(otherRoutes, greenmileRoutes);
}


Greenmile::TimeIntervalError Greenmile::timeIntervalHasError
(const QDateTime &minQueryDateTime, const QDateTime &maxQueryDateTime)
{
    if(minQueryDateTime.isNull() || maxQueryDateTime.isNull())
    {
        qDebug() << "Greenmile: Query datetimes are null,"
                    " returning empty route vector";

         return TimeIntervalError::nullInterval;
    }

    if(minQueryDateTime > maxQueryDateTime)
    {
        qDebug() << "Greenmile: Minimum time greater than max "
                    " time...  returning empty route vector";

         return TimeIntervalError::minGteMax;
    }

    return TimeIntervalError::noError;
}

void Greenmile::makeDateIntervalForQuery(const QVector<Route> &r)
{
     minQueryDate_ = QDate();
     maxQueryDate_ = QDate();
     QVector<Route> rte = r;

     if(rte.isEmpty())
     {
         qDebug() << "Greenmile: cannot create query interval from empty vector";
         return;
     }

     std::sort(rte.begin(), rte.end(), [](Route r1, Route r2) ->\
             bool {return r1.getRouteDate() < r2.getRouteDate();});

     minQueryDate_ = rte.first().getRouteDate();
     maxQueryDate_ = rte.last().getRouteDate();

     qDebug() << minQueryDate_.toString(Qt::ISODate);
     qDebug() << maxQueryDate_.toString(Qt::ISODate);
}

void Greenmile::makeDateTimeIntervalForQuery(const QVector<Route> &r)
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
            bool {return r1.getRouteBaselineDeparture() < r2.getRouteBaselineDeparture();});

    minQueryDateTime_ = rte.first().getRouteBaselineDeparture().addMSecs(-timeIntervalPaddingMsec_);
    maxQueryDateTime_ = rte.last().getRouteBaselineDeparture().addMSecs(timeIntervalPaddingMsec_);
    qDebug() << minQueryDateTime_.toString(Qt::ISODate);
    qDebug() << maxQueryDateTime_.toString(Qt::ISODate);
}

QByteArray Greenmile::queryGreenmileBaselineStartTime(const QDateTime &begin, const QDateTime &end)
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

QByteArray Greenmile::queryGreenmileRouteDate(const QDate &begin, const QDate &end)
{
    loadHeadersFromJson();
    QJsonObject postBodyJson;
    QJsonArray criteriaChain;

    QJsonObject evaluationStatement;
    QJsonArray logicalConditions;

    QJsonObject queryFromDateTime;
    queryFromDateTime["attr"] = QString("date");
    queryFromDateTime["gte"] = begin.toString(Qt::ISODate);

    QJsonObject queryToDateTime;
    queryToDateTime["attr"] = QString("date");
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

QVector<Route> Greenmile::buildRoutesFromGreenmileResponse(const QByteArray &gmResponse)
{
    Route route;
    QVector<Route> routes;
    QJsonArray routeArray = QJsonDocument::fromJson(gmResponse).array();

    for(auto jsonArrayValue: routeArray)
    {
        route = Route();

        QJsonObject jsonRoute = jsonArrayValue.toObject();

        route.setField(jsonRoute["key"].toString(), routeEnum::key);

        QJsonArray jsonArrayEquipmentAssignments =
                jsonRoute["equipmentAssignments"].toArray();
        QJsonArray jsonArrayDriverAssignments =
                jsonRoute["driverAssignments"].toArray();

        for(auto jsonArrayValue: jsonArrayEquipmentAssignments)
        {
            QJsonObject jsonEquipmentAssignment =\
                    jsonArrayValue.toObject();

            QJsonObject  jsonEquipment = jsonEquipmentAssignment["equipment"].toObject();
            route.setField(jsonEquipment["key"].toString(), routeEnum::truckNum);
        }

        for(auto jsonArrayValue: jsonArrayDriverAssignments)
        {
            QJsonObject jsonDriverAssignment = jsonArrayValue.toObject();
            QJsonObject jsonDriver = jsonDriverAssignment["driver"].toObject();
            route.setField(jsonDriver["key"].toString(), routeEnum::driverId);
            route.setField(jsonDriver["name"].toString(), routeEnum::driverName);
        }
        routes.append(route);
    }

    return routes;
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
