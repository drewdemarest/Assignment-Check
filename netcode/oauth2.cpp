#include "oauth2.h"

OAuth2::OAuth2(QObject *parent) : QObject(parent)
{

}

QSqlDatabase OAuth2::makeDatabase(/*path, sheetsScope, sheetAddress,
                                   clientId, projectId, authUri,
                                   tokenUri, auth_providerx509,
                                   clientSecret, redirectUris*/)
{
    QString sheetsScope;
    QString sheetsAddressBase;
    QString settingsPath_ = QApplication::applicationDirPath() + "/settings.db";
    qDebug() << settingsPath_;
    QSqlDatabase settings;
    settings = settings.addDatabase("QSQLITE", "settings");
    settings.setDatabaseName(settingsPath_);
    settings.open();
    QSqlQuery query(settings);

    query.prepare("SELECT value FROM masterRouteSettings WHERE variable = 'sheetsAddressBase'");
    query.exec();

    int i = 0;
    while(query.next())
    {
        qDebug() << query.value(i).toString() << i;
        i++;
//        if(query.value(0).toString() == "sheetsScope")
//            sheetsScope = query.value(1).toString();

//        if(query.value(0).toString() == "sheetsAddressBase")
//            sheetsAddressBase = query.value(2).toString();
    }

    qDebug() << sheetsAddressBase << sheetsScope;
    query.clear();

    return settings;
//    settings.close();
//    settings = QSqlDatabase();
//    settings.removeDatabase("settings");
}
