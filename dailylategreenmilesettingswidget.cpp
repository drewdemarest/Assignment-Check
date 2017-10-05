#include "dailylategreenmilesettingswidget.h"
#include "ui_dailylategreenmilesettingswidget.h"

DailyLateGreenmileSettingsWidget::DailyLateGreenmileSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DailyLateGreenmileSettingsWidget)
{
    ui->setupUi(this);
}

DailyLateGreenmileSettingsWidget::~DailyLateGreenmileSettingsWidget()
{
    delete ui;
}
