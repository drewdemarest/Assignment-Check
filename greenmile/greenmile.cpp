#include "greenmile.h"

Greenmile::Greenmile(QObject *parent) : QObject(parent)
{
    getRoutesForTimeInterval(QDateTime::currentDateTime().addSecs(-18000).toUTC(), QDateTime::currentDateTime().toUTC());
}

Greenmile::~Greenmile()
{

}

QVector<Route> Greenmile::getRoutesForTimeInterval(const QDateTime &begin, const QDateTime &end)
{
    QByteArray response = queryGreenmile(begin, end);
    qDebug() << response;
    QVector<Route> derp;
    return derp;
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

    qDebug() << QJsonDocument(postBodyJson).toJson(QJsonDocument::Compact);

    body = QJsonDocument(postBodyJson).toJson(QJsonDocument::Compact);
    return greenmileConnection->postRequest(headers, gmRouteIntervalAddress, body);
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
