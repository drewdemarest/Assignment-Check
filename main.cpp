#include "mainwindow.h"
#include "netcode/oauth2.h"
#include <QSqlDatabase>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    OAuth2 derp;
<<<<<<< HEAD
    derp.commitSettingsToDatabase();
=======
    QSqlDatabase settings =  derp.makeDatabase();
    settings.close();
    settings = QSqlDatabase();
    settings.removeDatabase("settings");
>>>>>>> testing

    return a.exec();
}
