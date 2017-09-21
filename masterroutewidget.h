#ifndef MASTERROUTEWIDGET_H
#define MASTERROUTEWIDGET_H

#include <QWidget>
#include <QtCore>
#include "routewidget.h"
#include "masterroute/masterroute.h"

namespace Ui {
class MasterRouteWidget;
}

class MasterRouteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MasterRouteWidget(QWidget *parent = 0);
    ~MasterRouteWidget();

    bool isCurrentlycurrentlyBuildingWidgets();
    MasterRoute *getMasterRoute();

private:
    Ui::MasterRouteWidget *ui;
    QStringListModel *mondayModel = new QStringListModel();
    QStringListModel *tuesdayModel = new QStringListModel();
    QStringListModel *wednesdayModel = new QStringListModel();
    QStringListModel *thursdayModel = new QStringListModel();
    QStringListModel *fridayModel = new QStringListModel();
    QStringListModel *saturdayModel = new QStringListModel();
    QStringListModel *sundayModel = new QStringListModel();

    bool currentlyBuildingWidgets = false;

    QVector<RouteWidget*> routeWidgets;
    MasterRoute *mrs = new MasterRoute(this);

    void buildRouteModels();

public slots:
    void buildWidgets();

private slots:
    void setMondayMRSStackIndex(const QModelIndex &mdlIdx);
    void setTuesdayMRSStackIndex(const QModelIndex &mdlIdx);
    void setWednesdayMRSStackIndex(const QModelIndex &mdlIdx);
    void setThursdayMRSStackIndex(const QModelIndex &mdlIdx);
    void setFridayMRSStackIndex(const QModelIndex &mdlIdx);
    void setSaturdayMRSStackIndex(const QModelIndex &mdlIdx);
    void setSundayMRSStackIndex(const QModelIndex &mdlIdx);

    void mondaySearch(const QString &toFind);
};

#endif // MASTERROUTEWIDGET_H
