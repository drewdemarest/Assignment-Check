#include "masterroutewidget.h"
#include "ui_masterroutewidget.h"

MasterRouteWidget::MasterRouteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MasterRouteWidget)
{
    ui->setupUi(this);

        connect(ui->loadRoutesButton, &QPushButton::clicked, this, &MasterRouteWidget::buildWidgets);
        connect(ui->abortButton, &QPushButton::clicked, QApplication::instance(), &QCoreApplication::quit);
}

MasterRouteWidget::~MasterRouteWidget()
{
    delete ui;
}

bool MasterRouteWidget::isCurrentlycurrentlyBuildingWidgets()
{
    return currentlyBuildingWidgets;
}

MasterRoute* MasterRouteWidget::getMasterRoute()
{
    return mrs;
}

void MasterRouteWidget::buildWidgets()
{
    ui->stackedWidget->setCurrentIndex(1);

    buildRouteModels();
    while(currentlyBuildingWidgets)
    {
       qApp->processEvents();
    }

    connect(ui->mondayMRSView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MasterRouteWidget::setMondayMRSStackIndex);
    connect(ui->tuesdayMRSView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MasterRouteWidget::setTuesdayMRSStackIndex);
    connect(ui->wednesdayMRSView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MasterRouteWidget::setWednesdayMRSStackIndex);
    connect(ui->thursdayMRSView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MasterRouteWidget::setThursdayMRSStackIndex);
    connect(ui->fridayMRSView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MasterRouteWidget::setFridayMRSStackIndex);
    connect(ui->saturdayMRSView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MasterRouteWidget::setSaturdayMRSStackIndex);
    connect(ui->sundayMRSView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MasterRouteWidget::setSundayMRSStackIndex);

    connect(ui->mondayRouteSearch, &QLineEdit::textChanged, this, &MasterRouteWidget::mondaySearch);

    ui->stackedWidget->setCurrentIndex(2);
}

void MasterRouteWidget::buildRouteModels()
{
    currentlyBuildingWidgets = true;
    QStringList routeKeys;

    mrs->buildAllRoutes();
    //mrs->buildMondayRoutes();
    for(auto t : mrs->getMondayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->mondayMRSStack->addWidget(routeWidgets.last());
    }

    mondayModel->setStringList(routeKeys);
    ui->mondayMRSView->setModel(mondayModel);

    routeKeys.clear();
    for(auto t : mrs->getTuesdayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->tuesdayMRSStack->addWidget(routeWidgets.last());
    }
    tuesdayModel->setStringList(routeKeys);
    ui->tuesdayMRSView->setModel(tuesdayModel);

    routeKeys.clear();
    for(auto t : mrs->getWednesdayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->wednesdayMRSStack->addWidget(routeWidgets.last());
    }

    wednesdayModel->setStringList(routeKeys);
    ui->wednesdayMRSView->setModel(wednesdayModel);

    routeKeys.clear();
    for(auto t : mrs->getThursdayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->thursdayMRSStack->addWidget(routeWidgets.last());
    }

    thursdayModel->setStringList(routeKeys);
    ui->thursdayMRSView->setModel(thursdayModel);

    routeKeys.clear();
    for(auto t : mrs->getFridayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->fridayMRSStack->addWidget(routeWidgets.last());
    }

    fridayModel->setStringList(routeKeys);
    ui->fridayMRSView->setModel(fridayModel);

    routeKeys.clear();
    for(auto t : mrs->getSaturdayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->saturdayMRSStack->addWidget(routeWidgets.last());
    }
    saturdayModel->setStringList(routeKeys);
    ui->saturdayMRSView->setModel(saturdayModel);

    routeKeys.clear();
    for(auto t : mrs->getSundayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->sundayMRSStack->addWidget(routeWidgets.last());
    }

    sundayModel->setStringList(routeKeys);
    ui->sundayMRSView->setModel(sundayModel);
    currentlyBuildingWidgets = false;

    return;
}

void MasterRouteWidget::setMondayMRSStackIndex(const QModelIndex &mdlIdx)
{
    ui->mondayMRSStack->setCurrentIndex(mdlIdx.row());
    return;
}

void MasterRouteWidget::setTuesdayMRSStackIndex(const QModelIndex &mdlIdx)
{
    ui->tuesdayMRSStack->setCurrentIndex(mdlIdx.row());
    return;
}

void MasterRouteWidget::setWednesdayMRSStackIndex(const QModelIndex &mdlIdx)
{
    ui->wednesdayMRSStack->setCurrentIndex(mdlIdx.row());
    return;
}

void MasterRouteWidget::setThursdayMRSStackIndex(const QModelIndex &mdlIdx)
{
    ui->thursdayMRSStack->setCurrentIndex(mdlIdx.row());
    return;
}

void MasterRouteWidget::setFridayMRSStackIndex(const QModelIndex &mdlIdx)
{
    ui->fridayMRSStack->setCurrentIndex(mdlIdx.row());
    return;
}

void MasterRouteWidget::setSaturdayMRSStackIndex(const QModelIndex &mdlIdx)
{
    ui->saturdayMRSStack->setCurrentIndex(mdlIdx.row());
    return;
}

void MasterRouteWidget::setSundayMRSStackIndex(const QModelIndex &mdlIdx)
{
    ui->sundayMRSStack->setCurrentIndex(mdlIdx.row());
    return;
}

void MasterRouteWidget::mondaySearch(const QString &toFind)
{
    auto found = mondayModel->match(mondayModel->index(0, 0), Qt::DisplayRole, QVariant::fromValue(toFind), -1, Qt::MatchStartsWith);
    if(!found.isEmpty())
    {
        ui->mondayMRSView->selectionModel()->select(found.at(0), QItemSelectionModel::ClearAndSelect);
        ui->mondayMRSView->scrollTo(found.at(0));
    }
}
