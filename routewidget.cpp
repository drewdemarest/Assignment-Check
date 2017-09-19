#include "routewidget.h"
#include "ui_routewidget.h"

RouteWidget::RouteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RouteWidget)
{
    ui->setupUi(this);
}

RouteWidget::RouteWidget(const Route &r, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RouteWidget)
{
    ui->setupUi(this);
    setRoute(r);
}


RouteWidget::~RouteWidget()
{
    delete ui;
}

void RouteWidget::setRoute(Route r)
{
    ui->driverNameEdit->setText(r.getDriverName());
    ui->driverNumEdit->setText(r.getDriverId());
    ui->routeEdit->setText(r.getKey());
    ui->trailerNumEdit->setText(r.getTrailerNumber());
    ui->truckNumEdit->setText(r.getTruckNumber());
}
