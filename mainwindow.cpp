#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList routePrec = {"route", "junk", "driver", "powerUnit", "trailer", "junk", "notes"};
    mrs.setRouteInfoPrecedence(routePrec);
    mrs.buildRoutes();
}

MainWindow::~MainWindow()
{
    delete ui;
}
