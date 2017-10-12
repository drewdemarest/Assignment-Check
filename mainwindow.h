#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "routewidget.h"
#include "dailylategreenmilewidget.h"
#include <masterroutewidget.h>
#include <greenmilewidget.h>
#include <greenmile/greenmile.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    MasterRouteWidget *truckKeyWidget = new MasterRouteWidget(this);
    GreenmileWidget *greenMileValidator = new GreenmileWidget(this);
    DailyLateGreenmileWidget *dailyLateGreenmileValidator = new DailyLateGreenmileWidget(this);
};

#endif // MAINWINDOW_H
