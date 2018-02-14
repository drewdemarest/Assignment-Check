#include "mainwindow.h"
#include "netcode/oauth2.h"
#include <QSqlDatabase>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //QString(QApplication::applicationDirPath()+ "/client.json"
    OAuth2 derp(QString(QApplication::applicationDirPath() + "/oauth2Settings.db"), QString(QApplication::applicationDirPath()+ "/client.json"));
    derp.get();
    return a.exec();
}
