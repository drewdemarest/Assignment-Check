#ifndef ROUTEWIDGET_H
#define ROUTEWIDGET_H

#include <QWidget>
#include "masterroute/route.h"

namespace Ui {
class RouteWidget;
}

class RouteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RouteWidget(QWidget *parent = 0);
    explicit RouteWidget(const Route &r, QWidget *parent = 0);
    ~RouteWidget();

    void setRoute(Route r);

private:
    Ui::RouteWidget *ui;
};

#endif // ROUTEWIDGET_H
