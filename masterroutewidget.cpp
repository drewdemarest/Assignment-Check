#include "masterroutewidget.h"
#include "ui_masterroutewidget.h"

MasterRouteWidget::MasterRouteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MasterRouteWidget)
{
    ui->setupUi(this);
}

MasterRouteWidget::~MasterRouteWidget()
{
    delete ui;
}
