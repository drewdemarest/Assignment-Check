#ifndef ROUTEDIFFERENCEMODEL_H
#define ROUTEDIFFERENCEMODEL_H

#include <QAbstractTableModel>
#include "routedifference.h"

class RouteDifferenceModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit RouteDifferenceModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void addRouteDifferenceVector(const QVector<RouteDifference> &routeDiffs);
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void clear();

private:
    QVector<RouteDifference> routeDifferences;
    int routeDifferenceMemberCount = 8;

};

#endif // ROUTEDIFFERENCEMODEL_H
