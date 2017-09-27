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

    if(orientation == Qt::Horizontal)
    {
        switch(section)
        {
        case 0:
            return QString("Uploaded to Greenmile");
        case 1:
            return QString("Exists in Master Route Sheet");
        case 2:
            return QString("Driver Mismatch");
        case 3:
            return QString("Truck Mismatch");
        case 4:
            return QString("Master Route Truck");
        case 5:
            return QString("Master Route Truck");
        case 6:
            return QString("Greenmile Truck");
        case 7:
            return QString("Greenmile Driver");
        }
    }

    if(orientation == Qt::Vertical)
    {
        return routeDifferences.at(section).routeKey;
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

    return routeDifferences.count();
    // FIXME: Implement me!
}

int RouteDifferenceModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;


    return routeDifferenceMemberCount;
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
        return routeDiffAtRow.routeExistsInGreenmile;
    case 1:
        return routeDiffAtRow.routeExistsInMasterRoute;
    case 2:
        return routeDiffAtRow.driverMismatch;
    case 3:
        return routeDiffAtRow.truckMismatch;
    case 4:
        return routeDiffAtRow.masterRouteTruck;
    case 5:
        return routeDiffAtRow.masterRouteDriverID;
    case 6:
        return routeDiffAtRow.greenmileTruck;
    case 7:
        return routeDiffAtRow.greenmileDriverID;
    }
    // FIXME: Implement me!

    return QVariant();
}
