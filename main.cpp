#include "mainwindow.h"
#include "netcode/oauth2.h"
#include <QSqlDatabase>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    OAuth2 derp(QString(QApplication::applicationDirPath() + "/oauth2Settings.db"));
    derp.get();
    return a.exec();
}
