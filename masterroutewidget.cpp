#include "masterroutewidget.h"
#include "ui_masterroutewidget.h"

MasterRouteWidget::MasterRouteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MasterRouteWidget)
{
    ui->setupUi(this);

        connect(ui->loadRoutesButton, &QPushButton::clicked, this, &MasterRouteWidget::buildWidgets);
        connect(ui->abortButton, &QPushButton::clicked, mrs, &MasterRoute::abort);
        connect(ui->refreshMRSButton, &QPushButton::clicked, this, &MasterRouteWidget::refreshRoutes);
        connect(ui->refreshMRSButton, &QPushButton::clicked, mrs, &MasterRoute::tryNetworkAgain);
}

MasterRouteWidget::~MasterRouteWidget()
{
    delete ui;
    mondayModel->deleteLater();
    tuesdayModel->deleteLater();
    wednesdayModel->deleteLater();
    thursdayModel->deleteLater();
    fridayModel->deleteLater();
    saturdayModel->deleteLater();
    sundayModel->deleteLater();

    for(auto rw : routeWidgets)
    {
        rw->deleteLater();
    }
    routeWidgets.clear();

    mrs->deleteLater();
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
    connect(ui->tuesdayRouteSearch, &QLineEdit::textChanged, this, &MasterRouteWidget::tuesdaySearch);
    connect(ui->wednesdayRouteSearch, &QLineEdit::textChanged, this, &MasterRouteWidget::wednesdaySearch);
    connect(ui->thursdayRouteSearch, &QLineEdit::textChanged, this, &MasterRouteWidget::thursdaySearch);
    connect(ui->fridayRouteSearch, &QLineEdit::textChanged, this, &MasterRouteWidget::fridaySearch);
    connect(ui->saturdayRouteSearch, &QLineEdit::textChanged, this, &MasterRouteWidget::saturdaySearch);
    connect(ui->sundayRouteSearch, &QLineEdit::textChanged, this, &MasterRouteWidget::sundaySearch);

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

void MasterRouteWidget::tuesdaySearch(const QString &toFind)
{
    auto found = tuesdayModel->match(tuesdayModel->index(0, 0), Qt::DisplayRole, QVariant::fromValue(toFind), -1, Qt::MatchStartsWith);
    if(!found.isEmpty())
    {
        ui->tuesdayMRSView->selectionModel()->select(found.at(0), QItemSelectionModel::ClearAndSelect);
        ui->tuesdayMRSView->scrollTo(found.at(0));
    }
}

void MasterRouteWidget::wednesdaySearch(const QString &toFind)
{
    auto found = wednesdayModel->match(wednesdayModel->index(0, 0), Qt::DisplayRole, QVariant::fromValue(toFind), -1, Qt::MatchStartsWith);
    if(!found.isEmpty())
    {
        ui->wednesdayMRSView->selectionModel()->select(found.at(0), QItemSelectionModel::ClearAndSelect);
        ui->wednesdayMRSView->scrollTo(found.at(0));
    }
}

void MasterRouteWidget::thursdaySearch(const QString &toFind)
{
    auto found = thursdayModel->match(thursdayModel->index(0, 0), Qt::DisplayRole, QVariant::fromValue(toFind), -1, Qt::MatchStartsWith);
    if(!found.isEmpty())
    {
        ui->thursdayMRSView->selectionModel()->select(found.at(0), QItemSelectionModel::ClearAndSelect);
        ui->thursdayMRSView->scrollTo(found.at(0));
    }
}

void MasterRouteWidget::fridaySearch(const QString &toFind)
{
    auto found = fridayModel->match(fridayModel->index(0, 0), Qt::DisplayRole, QVariant::fromValue(toFind), -1, Qt::MatchStartsWith);
    if(!found.isEmpty())
    {
        ui->fridayMRSView->selectionModel()->select(found.at(0), QItemSelectionModel::ClearAndSelect);
        ui->fridayMRSView->scrollTo(found.at(0));
    }
}

void MasterRouteWidget::saturdaySearch(const QString &toFind)
{
    auto found = saturdayModel->match(saturdayModel->index(0, 0), Qt::DisplayRole, QVariant::fromValue(toFind), -1, Qt::MatchStartsWith);
    if(!found.isEmpty())
    {
        ui->saturdayMRSView->selectionModel()->select(found.at(0), QItemSelectionModel::ClearAndSelect);
        ui->saturdayMRSView->scrollTo(found.at(0));
    }
}


void MasterRouteWidget::sundaySearch(const QString &toFind)
{
    auto found = sundayModel->match(sundayModel->index(0, 0), Qt::DisplayRole, QVariant::fromValue(toFind), -1, Qt::MatchStartsWith);
    if(!found.isEmpty())
    {
        ui->sundayMRSView->selectionModel()->select(found.at(0), QItemSelectionModel::ClearAndSelect);
        ui->sundayMRSView->scrollTo(found.at(0));
    }
}

void MasterRouteWidget::refreshRoutes()
{
    for(int i = ui->mondayMRSStack->count(); i >= 0; i--){
        ui->mondayMRSStack->removeWidget(ui->mondayMRSStack->widget(i));
    }

    for(int i = ui->tuesdayMRSStack->count(); i >= 0; i--)
        ui->tuesdayMRSStack->removeWidget(ui->tuesdayMRSStack->widget(i));

    for(int i = ui->wednesdayMRSStack->count(); i >= 0; i--)
        ui->wednesdayMRSStack->removeWidget(ui->wednesdayMRSStack->widget(i));

    for(int i = ui->thursdayMRSStack->count(); i >= 0; i--)
        ui->thursdayMRSStack->removeWidget(ui->thursdayMRSStack->widget(i));

    for(int i = ui->fridayMRSStack->count(); i >= 0; i--)
        ui->fridayMRSStack->removeWidget(ui->fridayMRSStack->widget(i));

    for(int i = ui->saturdayMRSStack->count(); i >= 0; i--)
        ui->saturdayMRSStack->removeWidget(ui->saturdayMRSStack->widget(i));

    for(int i = ui->sundayMRSStack->count(); i >= 0; i--)
        ui->sundayMRSStack->removeWidget(ui->sundayMRSStack->widget(i));

    for(auto rw: routeWidgets)
    {
        rw->deleteLater();
    }
    routeWidgets.clear();

    mondayModel->setStringList(QStringList());
    tuesdayModel->setStringList(QStringList());
    wednesdayModel->setStringList(QStringList());
    thursdayModel->setStringList(QStringList());
    fridayModel->setStringList(QStringList());
    saturdayModel->setStringList(QStringList());
    sundayModel->setStringList(QStringList());

    buildWidgets();
}
