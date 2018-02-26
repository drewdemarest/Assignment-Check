#include "mainwindow.h"
#include "netcode/oauth2.h"
#include <QSqlDatabase>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon(":/gmac.png"));
    w.show();

    return a.exec();
}
