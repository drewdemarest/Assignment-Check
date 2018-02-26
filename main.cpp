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
    //OAuth2 derp(QString(QApplication::applicationDirPath() + "/oauth2Settings.db"), QString(QApplication::applicationDirPath()+ "/client.json"), "https://www.googleapis.com/auth/spreadsheets.readonly");
    //qDebug() << derp.get("https://sheets.googleapis.com/v4/spreadsheets/1KA7c9bbG2p4f8SFe5ibbkIycwt0wukRe2_xpTB3SI6A/values/Route Start Times");
    return a.exec();
}
