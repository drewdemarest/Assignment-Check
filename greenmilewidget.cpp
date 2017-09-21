#include "greenmilewidget.h"
#include "ui_greenmilewidget.h"

GreenmileWidget::GreenmileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GreenmileWidget)
{
    ui->setupUi(this);
}

GreenmileWidget::~GreenmileWidget()
{
    delete ui;
}
