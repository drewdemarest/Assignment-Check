#ifndef GREENMILEWIDGET_H
#define GREENMILEWIDGET_H

#include <QtCore>
#include <QTableView>
#include <QWidget>
#include <greenmile/greenmile.h>
#include <greenmile/routedifference.h>
#include <greenmile/routedifferencemodel.h>
#include <masterroute/masterroute.h>

namespace gmWidgetPages
{
    enum {startPage, loadingPage, reportPage};
}

namespace Ui {
class GreenmileWidget;
}

class GreenmileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GreenmileWidget(QWidget *parent = 0);
    ~GreenmileWidget();

private:
    Ui::GreenmileWidget *ui;
    MasterRoute *mrs = new MasterRoute();
    Greenmile *gm = new Greenmile();
    RouteDifferenceModel *routeDiffModel = new RouteDifferenceModel();
    QVector<RouteDifference> routeDifferences;


private slots:
    void todayGMButtonPressed();
    void mondayGMButtonPressed();
    void tuesdayGMButtonPressed();
    void wednesdayGMButtonPressed();
    void thursdayGMButtonPressed();
    void fridayGMButtonPressed();
    void saturdayGMButtonPressed();
    void sundayGMButtonPressed();

    void runReportAgain();
};

#endif // GREENMILEWIDGET_H
