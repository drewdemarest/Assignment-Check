#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "routewidget.h"
#include <netcode/oauthnetconnect.h>
#include <masterroute/masterroute.h>

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
    QStringListModel *mondayModel = new QStringListModel();
    QStringListModel *tuesdayModel = new QStringListModel();
    QStringListModel *wednesdayModel = new QStringListModel();
    QStringListModel *thursdayModel = new QStringListModel();
    QStringListModel *fridayModel = new QStringListModel();
    QStringListModel *saturdayModel = new QStringListModel();
    QStringListModel *sundayModel = new QStringListModel();

    QVector<RouteWidget*> routeWidgets;
    MasterRoute mrs;

    void buildWidgets();
    void applyRoutesToWidgets(const QVector<Route> &routes);

private slots:
    void setMondayStackedIndex(const QModelIndex &mdlIdx);
    void setTuesdayStackedIndex(const QModelIndex &mdlIdx);
    void setWednesdayStackedIndex(const QModelIndex &mdlIdx);
    void setThursdayStackedIndex(const QModelIndex &mdlIdx);
    void setFridayStackedIndex(const QModelIndex &mdlIdx);
    void setSaturdayStackedIndex(const QModelIndex &mdlIdx);
    void setSundayStackedIndex(const QModelIndex &mdlIdx);

    };

#endif // MAINWINDOW_H
