#ifndef DAILYLATEGREENMILEWIDGET_H
#define DAILYLATEGREENMILEWIDGET_H

#include <QtCore>
#include <QTableView>
#include <QWidget>
#include <greenmile/greenmile.h>
#include <greenmile/routedifference.h>
#include <greenmile/routedifferencemodel.h>
#include <dailylatemasterroute/dailylatemasterroute.h>
#include <masterroute/masterroute.h>

namespace dailyLateGMWidgetPages
{
    enum {startPage, loadingPage, reportPage};
}

namespace Ui {
class DailyLateGreenmileWidget;
}

class DailyLateGreenmileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DailyLateGreenmileWidget(QWidget *parent = 0);
    ~DailyLateGreenmileWidget();

private:
    Ui::DailyLateGreenmileWidget *ui;
    DailyLateMasterRoute *dlmrs = new DailyLateMasterRoute();
    Greenmile *gm = new Greenmile();
    RouteDifferenceModel *routeDiffModel = new RouteDifferenceModel();
    QVector<RouteDifference> routeDifferences;

private slots:
    void todayGMButtonPressed();
    void runReportAgain();
};

#endif // DAILYLATEGREENMILEWIDGET_H
