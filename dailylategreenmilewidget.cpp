#include "dailylategreenmilewidget.h"
#include "ui_dailylategreenmilewidget.h"

DailyLateGreenmileWidget::DailyLateGreenmileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DailyLateGreenmileWidget)
{
    ui->setupUi(this);
    connect(ui->todayGMButton, &QPushButton::clicked, this, &DailyLateGreenmileWidget::todayGMButtonPressed);
    connect(ui->gmRunReportAgain, &QPushButton::clicked, this, &DailyLateGreenmileWidget::runReportAgain);
    connect(ui->gmAbortButton, &QPushButton::clicked, dlmrs, &DailyLateMasterRoute::abort);
}

DailyLateGreenmileWidget::~DailyLateGreenmileWidget()
{
    delete ui;

    dlmrs->deleteLater();
    gm->deleteLater();
    routeDiffModel->deleteLater();
}

void DailyLateGreenmileWidget::todayGMButtonPressed()
{
    ui->gmStackedWidget->setCurrentIndex(dailyLateGMWidgetPages::loadingPage);

    //IMPLEMENT DLMRS ROUTE REMOVAL HERE.
    dlmrs->buildRoutes();

    routeDifferences = gm->compareDLMRSToGreenmileRoutes(dlmrs->getRoutes());
    for(auto rd: routeDifferences)
    {
        rd.printDebug();
    }

    routeDiffModel->addRouteDifferenceVector(routeDifferences);
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->routeDiffTableView->resizeColumnsToContents();
    ui->gmStackedWidget->setCurrentIndex(dailyLateGMWidgetPages::reportPage);
}

void DailyLateGreenmileWidget::runReportAgain()
{
    routeDiffModel->clear();
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->gmStackedWidget->setCurrentIndex(dailyLateGMWidgetPages::startPage);
}
