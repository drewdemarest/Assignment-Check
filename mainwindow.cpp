#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString scope = "https://www.googleapis.com/auth/spreadsheets.readonly";
    QString address = "https://sheets.googleapis.com/v4/spreadsheets/1KA7c9bbG2p4f8SFe5ibbkIycwt0wukRe2_xpTB3SI6A/values/Monday";
    QString credFilePath = QApplication::applicationDirPath() + "/client.json";
    qDebug() << credFilePath;
    qDebug() << address;
    oauthConn->buildOAuth(scope, address, credFilePath);
    qDebug() << oauthConn->get();
}

MainWindow::~MainWindow()
{
    delete ui;
}
