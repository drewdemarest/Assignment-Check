#include "dailylategreenmilewidget.h"
#include "ui_dailylategreenmilewidget.h"

DailyLateGreenmileWidget::DailyLateGreenmileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DailyLateGreenmileWidget)
{
    ui->setupUi(this);
}

DailyLateGreenmileWidget::~DailyLateGreenmileWidget()
{
    delete ui;
}
