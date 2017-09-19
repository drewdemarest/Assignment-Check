#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    buildWidgets();

//    mrs.buildMondayRoutes();
//    applyRoutesToWidgets(mrs.getMondayRoutes());
    connect(ui->mondayList->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::setMondayStackedIndex);
    connect(ui->tuesdayList->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::setTuesdayStackedIndex);
    connect(ui->wednesdayList->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::setWednesdayStackedIndex);
    connect(ui->thursdayList->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::setThursdayStackedIndex);
    connect(ui->fridayList->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::setFridayStackedIndex);
    connect(ui->saturdayList->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::setSaturdayStackedIndex);
    connect(ui->sundayList->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::setSundayStackedIndex);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::buildWidgets()
{
    QStringList routeKeys;

    mrs.buildAllRoutes();
    //mrs.buildMondayRoutes();
    for(auto t : mrs.getMondayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->mondayStacked->addWidget(routeWidgets.last());
    }

    mondayModel->setStringList(routeKeys);
    ui->mondayList->setModel(mondayModel);

    routeKeys.clear();
    for(auto t : mrs.getTuesdayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->tuesdayStacked->addWidget(routeWidgets.last());
    }
    tuesdayModel->setStringList(routeKeys);
    ui->tuesdayList->setModel(tuesdayModel);

    routeKeys.clear();
    for(auto t : mrs.getWednesdayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->wednesdayStacked->addWidget(routeWidgets.last());
    }

    wednesdayModel->setStringList(routeKeys);
    ui->wednesdayList->setModel(wednesdayModel);

    routeKeys.clear();
    for(auto t : mrs.getThursdayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->thursdayStacked->addWidget(routeWidgets.last());
    }

    thursdayModel->setStringList(routeKeys);
    ui->thursdayList->setModel(thursdayModel);

    routeKeys.clear();
    for(auto t : mrs.getFridayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->fridayStacked->addWidget(routeWidgets.last());
    }

    fridayModel->setStringList(routeKeys);
    ui->fridayList->setModel(fridayModel);

    routeKeys.clear();
    for(auto t : mrs.getSaturdayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->saturdayStacked->addWidget(routeWidgets.last());
    }
    saturdayModel->setStringList(routeKeys);
    ui->saturdayList->setModel(saturdayModel);

    routeKeys.clear();
    for(auto t : mrs.getSundayRoutes())
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->sundayStacked->addWidget(routeWidgets.last());
    }

    sundayModel->setStringList(routeKeys);
    ui->sundayList->setModel(sundayModel);
}

void MainWindow::applyRoutesToWidgets(const QVector<Route> &routes)
{
    QStringList routeKeys;
    for(auto t : routes)
    {
        routeKeys << t.getKey();
        routeWidgets.append(new RouteWidget(t, this));
        ui->mondayStacked->addWidget(routeWidgets.last());
    }

    mondayModel->setStringList(routeKeys);
    ui->mondayList->setModel(mondayModel);
}

void MainWindow::setMondayStackedIndex(const QModelIndex &mdlIdx)
{
    ui->mondayStacked->setCurrentIndex(mdlIdx.row());
    return;
}

void MainWindow::setTuesdayStackedIndex(const QModelIndex &mdlIdx)
{
    ui->tuesdayStacked->setCurrentIndex(mdlIdx.row());
    return;
}

void MainWindow::setWednesdayStackedIndex(const QModelIndex &mdlIdx)
{
    ui->wednesdayStacked->setCurrentIndex(mdlIdx.row());
    return;
}

void MainWindow::setThursdayStackedIndex(const QModelIndex &mdlIdx)
{
    ui->thursdayStacked->setCurrentIndex(mdlIdx.row());
    return;
}

void MainWindow::setFridayStackedIndex(const QModelIndex &mdlIdx)
{
    ui->fridayStacked->setCurrentIndex(mdlIdx.row());
    return;
}

void MainWindow::setSaturdayStackedIndex(const QModelIndex &mdlIdx)
{
    ui->saturdayStacked->setCurrentIndex(mdlIdx.row());
    return;
}

void MainWindow::setSundayStackedIndex(const QModelIndex &mdlIdx)
{
    ui->sundayStacked->setCurrentIndex(mdlIdx.row());
    return;
}
