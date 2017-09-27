#include "greenmilewidget.h"
#include "ui_greenmilewidget.h"


GreenmileWidget::GreenmileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GreenmileWidget)
{
    ui->setupUi(this);
    connect(ui->todayGMButton, &QPushButton::clicked, this, &GreenmileWidget::todayGMButtonPressed);
    connect(ui->gmRunReportAgain, &QPushButton::clicked, this, &GreenmileWidget::todayGMButtonPressed);
}

GreenmileWidget::~GreenmileWidget()
{
    delete ui;
    mrs->deleteLater();
    gm->deleteLater();
}


void GreenmileWidget::todayGMButtonPressed()
{
    ui->stackedWidget->setCurrentIndex(gmWidgetPages::loadingPage);
    switch(QDate::currentDate().dayOfWeek())
    {
    case 0:
        qDebug() << "Invalid date in todayGMButtonPressed, check system settings.";
        ui->stackedWidget->setCurrentIndex(gmWidgetPages::reportPage);
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
    ui->stackedWidget->setCurrentIndex(gmWidgetPages::reportPage);
}

void GreenmileWidget::runReportAgain()
{
    ui->stackedWidget->setCurrentIndex(gmWidgetPages::startPage);
}
