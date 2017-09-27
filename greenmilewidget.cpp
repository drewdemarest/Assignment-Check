#include "greenmilewidget.h"
#include "ui_greenmilewidget.h"


GreenmileWidget::GreenmileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GreenmileWidget)
{
    ui->setupUi(this);
    connect(ui->todayGMButton, &QPushButton::clicked, this, &GreenmileWidget::todayGMButtonPressed);
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

void GreenmileWidget::runReportAgain()
{ 
    routeDiffModel->clear();
    ui->routeDiffTableView->setModel(routeDiffModel);
    ui->gmStackedWidget->setCurrentIndex(gmWidgetPages::startPage);
}
