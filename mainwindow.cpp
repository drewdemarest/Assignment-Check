#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->truckKeyGridLayout->addWidget(truckKeyWidget);
    ui->greenmileGridLayout->addWidget(greenMileValidator);
    ui->dailyLateGreenmileTabLayout->addWidget(dailyLateGreenmileValidator);

    connect(truckKeyWidget, &MasterRouteWidget::abort, QApplication::instance(), &QCoreApplication::quit);
}

MainWindow::~MainWindow()
{
    truckKeyWidget->deleteLater();
    greenMileValidator->deleteLater();
    dailyLateGreenmileValidator->deleteLater();

    delete ui;
}
