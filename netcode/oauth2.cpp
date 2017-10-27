#include "oauth2.h"

OAuth2::OAuth2(QObject *parent) : QObject(parent)
{

}

<<<<<<< HEAD
bool OAuth2::commitSettingsToDatabase(/*path, sheetsScope, sheetAddress,
                                              clientId, projectId, authUri,
                                              tokenUri, auth_providerx509,
                                              clientSecret, redirectUris,
                                              databaseFileName*/)
=======
QSqlDatabase OAuth2::makeDatabase(/*path, sheetsScope, sheetAddress,
                                   clientId, projectId, authUri,
                                   tokenUri, auth_providerx509,
                                   clientSecret, redirectUris*/)
>>>>>>> testing
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
<<<<<<< HEAD
        if(query.value(0).toString() == "sheetsScope")
            sheetsScope = query.value(1).toString();

        if(query.value(0).toString() == "sheetsAddressBase")
            sheetsAddressBase = query.value(2).toString();
=======
//        if(query.value(0).toString() == "sheetsScope")
//            sheetsScope = query.value(1).toString();

//        if(query.value(0).toString() == "sheetsAddressBase")
//            sheetsAddressBase = query.value(2).toString();
>>>>>>> testing
    }

    qDebug() << sheetsAddressBase << sheetsScope;
    query.clear();

<<<<<<< HEAD
    settings.close();
    settings = QSqlDatabase();
    settings.removeDatabase("settings");
    return settingsPath_;

}

=======
    return settings;
//    settings.close();
//    settings = QSqlDatabase();
//    settings.removeDatabase("settings");
}
>>>>>>> testing
