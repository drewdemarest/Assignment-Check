#include "greenmilewidget.h"
#include "ui_greenmilewidget.h"


GreenmileWidget::GreenmileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GreenmileWidget)
{
    ui->setupUi(this);
    connect(ui->todayGMButton, &QPushButton::clicked, this, &GreenmileWidget::todayGMButtonPressed);
    connect(ui->mondayGMButton, &QPushButton::clicked, this, &GreenmileWidget::mondayGMButtonPressed);
    connect(ui->tuesdayGMButton, &QPushButton::clicked, this, &GreenmileWidget::tuesdayGMButtonPressed);
    connect(ui->wednesdayGMButton, &QPushButton::clicked, this, &GreenmileWidget::wednesdayGMButtonPressed);
    connect(ui->thursdayGMButton, &QPushButton::clicked, this, &GreenmileWidget::thursdayGMButtonPressed);
    connect(ui->fridayGMButton, &QPushButton::clicked, this, &GreenmileWidget::fridayGMButtonPressed);
    connect(ui->saturdayGMButton, &QPushButton::clicked, this, &GreenmileWidget::saturdayGMButtonPressed);
    connect(ui->sundayGMButton, &QPushButton::clicked, this, &GreenmileWidget::sundayGMButtonPressed);
    connect(ui->gmRunReportAgain, &QPushButton::clicked, this, &GreenmileWidget::runReportAgain);
}

GreenmileWidget::~GreenmileWidget()
{
    delete ui;
    mrs->deleteLater();
    gm->deleteLater();
    routeDiffModel->deleteLater();
}


void GreenmileWidget::todayGMButtonPressed()
{
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::loadingPage);
    switch(QDate::currentDate().dayOfWeek())
    {
    case 0:
        qDebug() << "Invalid date in todayGMButtonPressed, check system settings.";
        ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::reportPage);
        return;
    case 1:
        mrs->buildMondayRoutes();
        routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getMondayRoutes());
        break;
    case 2:
        mrs->buildTuesdayRoutes();
        routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getTuesdayRoutes());
        break;
    case 3:
        mrs->buildWednesdayRoutes();
        routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getWednesdayRoutes());
        break;
    case 4:
        mrs->buildThursdayRoutes();
        routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getThursdayRoutes());
        break;
    case 5:
        mrs->buildFridayRoutes();
        routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getFridayRoutes());
        break;
    case 6:
        mrs->buildSaturdayRoutes();
        routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getSaturdayRoutes());
        break;
    case 7:
        mrs->buildSundayRoutes();
        routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getSundayRoutes());
        break;
    }

    routeDiffModel->addRouteDifferenceVector(routeDifferences);
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->routeDiffTableView->resizeColumnsToContents();
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::reportPage);
}

void GreenmileWidget::mondayGMButtonPressed()
{
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::loadingPage);
    mrs->buildMondayRoutes();
    routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getMondayRoutes());
    routeDiffModel->addRouteDifferenceVector(routeDifferences);
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->routeDiffTableView->resizeColumnsToContents();
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::reportPage);
}

void GreenmileWidget::tuesdayGMButtonPressed()
{
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::loadingPage);
    mrs->buildTuesdayRoutes();
    routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getTuesdayRoutes());
    routeDiffModel->addRouteDifferenceVector(routeDifferences);
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->routeDiffTableView->resizeColumnsToContents();
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::reportPage);
}

void GreenmileWidget::wednesdayGMButtonPressed()
{
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::loadingPage);
    mrs->buildWednesdayRoutes();
    routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getWednesdayRoutes());
    routeDiffModel->addRouteDifferenceVector(routeDifferences);
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->routeDiffTableView->resizeColumnsToContents();
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::reportPage);
}

void GreenmileWidget::thursdayGMButtonPressed()
{
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::loadingPage);
    mrs->buildThursdayRoutes();
    routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getThursdayRoutes());
    routeDiffModel->addRouteDifferenceVector(routeDifferences);
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->routeDiffTableView->resizeColumnsToContents();
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::reportPage);
}

void GreenmileWidget::fridayGMButtonPressed()
{
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::loadingPage);
    mrs->buildFridayRoutes();
    routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getFridayRoutes());
    routeDiffModel->addRouteDifferenceVector(routeDifferences);
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->routeDiffTableView->resizeColumnsToContents();
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::reportPage);
}

void GreenmileWidget::saturdayGMButtonPressed()
{
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::loadingPage);
    mrs->buildSaturdayRoutes();
    routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getSaturdayRoutes());
    routeDiffModel->addRouteDifferenceVector(routeDifferences);
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->routeDiffTableView->resizeColumnsToContents();
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::reportPage);
}

void GreenmileWidget::sundayGMButtonPressed()
{
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::loadingPage);
    mrs->buildSundayRoutes();
    routeDifferences = gm->compareRoutesToGreenmileRoutes(mrs->getSundayRoutes());
    routeDiffModel->addRouteDifferenceVector(routeDifferences);
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->routeDiffTableView->resizeColumnsToContents();
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::reportPage);
}

void GreenmileWidget::runReportAgain()
{ 
    routeDiffModel->clear();
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::startPage);
}
