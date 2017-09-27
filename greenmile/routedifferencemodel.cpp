#include "routedifferencemodel.h"

RouteDifferenceModel::RouteDifferenceModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant RouteDifferenceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    if( role != Qt::DisplayRole)
        return QVariant();

    switch(section)
    {
    case 0:
        return QString("Route Key");
    case 1:
        return QString("Uploaded to Greenmile");
    case 2:
        return QString("Exists in Master Route Sheet");
    case 3:
        return QString("Driver Mismatch");
    case 4:
        return QString("Truck Mismatch");
    case 5:
        return QString("Master Route Truck");
    case 6:
        return QString("Master Route Truck");
    case 7:
        return QString("Greenmile Truck");
    case 8:
        return QString("Greenmile Driver");
    }

    return QVariant();
}

void RouteDifferenceModel::addRouteDifferenceVector(const QVector<RouteDifference> &routeDiffs)
{
    routeDifferences = routeDiffs;
}

int RouteDifferenceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return routeDifferenceMemberCount;
    // FIXME: Implement me!
}

int RouteDifferenceModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return routeDifferences.count();
    // FIXME: Implement me!
}

QVariant RouteDifferenceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    RouteDifference routeDiffAtRow = routeDifferences.at(index.row());

    switch(index.column())
    {
    case 0:
        return routeDiffAtRow.routeKey;
    case 1:
        return routeDiffAtRow.routeExistsInGreenmile;
    case 2:
        return routeDiffAtRow.routeExistsInMasterRoute;
    case 3:
        return routeDiffAtRow.driverMismatch;
    case 4:
        return routeDiffAtRow.truckMismatch;
    case 5:
        return routeDiffAtRow.masterRouteTruck;
    case 6:
        return routeDiffAtRow.masterRouteDriverID;
    case 7:
        return routeDiffAtRow.greenmileTruck;
    case 8 :
        return routeDiffAtRow.greenmileDriverID;
    }
    // FIXME: Implement me!

    return QVariant();
}
