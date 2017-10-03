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
            return QString("Master Route Driver Name");
        case 6:
            return QString("Master Route Driver ID");
        case 7:
            return QString("Greenmile Truck");
        case 8:
            return QString("Greenmile Driver Name");
        case 9:
            return QString("Greenmile Driver ID");
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
    this->beginResetModel();
    routeDifferences = routeDiffs;
    this->endResetModel();
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

    if(role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignCenter | Qt::AlignVCenter);
    }

    if(role == Qt::CheckStateRole)
    {
        switch(index.column())
        {
        case 0:
            return bool(routeDiffAtRow.routeExistsInSourceB);
        case 1:
            return bool(routeDiffAtRow.routeExistsInSourceA);
        case 2:
            return bool(routeDiffAtRow.driverMismatch);
        case 3:
            return bool(routeDiffAtRow.truckMismatch);
        }
    }

    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case 4:
            return QString(routeDiffAtRow.powerUnitA);
        case 5:
            return QString(routeDiffAtRow.driverNameA);
        case 6:
            return QString(routeDiffAtRow.driverNumberA);
        case 7:
            return QString(routeDiffAtRow.powerUnitB);
        case 8:
            return QString(routeDiffAtRow.driverNameB);
        case 9:
            return QString(routeDiffAtRow.driverNumberB);
        }
    }
    // FIXME: Implement me!

    return QVariant();
}

void RouteDifferenceModel::clear()
{
    this->beginResetModel();
    routeDifferences.clear();
    this->endResetModel();
}
