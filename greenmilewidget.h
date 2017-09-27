#ifndef GREENMILEWIDGET_H
#define GREENMILEWIDGET_H

#include <QtCore>
#include <QWidget>
#include <greenmile/greenmile.h>
#include <greenmile/routedifference.h>
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
    QVector<RouteDifference> routeDifferences;

private slots:
    void todayGMButtonPressed();
    void runReportAgain();
};

#endif // GREENMILEWIDGET_H
