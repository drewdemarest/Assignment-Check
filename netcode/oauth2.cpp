#include "oauth2.h"

OAuth2::OAuth2(QObject *parent) : QObject(parent), Json2Sqlite()
{
    qDebug() << "SUMMONED!";

    oauth2Settings_ = loadSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
    qDebug() << oauth2Settings_;

//test code to make big old array from file

    QByteArray testQBA;
    QFile testArrFile(QString(QApplication::applicationDirPath() + "/tst.txt"));

    if(!testArrFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open test file";
    }

    while(!testArrFile.atEnd())
    {
        testQBA.append(testArrFile.readLine());
    }
    //testQBA.replace("'", QString(QStringLiteral("\\\'")).toUtf8());
    //qDebug() << testQBA;
    QJsonArray testArr = QJsonDocument::fromJson(testQBA).array();
    qDebug() << testArr.size();
    testArrFile.close();

//end test code

    saveArray(QString(QApplication::applicationDirPath() + "/test.db"), "dingo", "id", testArr);
}

OAuth2::OAuth2(QString dbPath, QObject *parent) : QObject(parent), Json2Sqlite()
{
    qDebug() << "SUMMONED! AND OVERLOADED, PHORRRWAR";

    oauth2Settings_["db_path"] = dbPath;
    oauth2Settings_ = loadSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_);
}


bool OAuth2::setCredentialsFromJsonFile(QString jsonCredPath)
{
    const QJsonObject credentials = makeJsonFromFile(jsonCredPath);
    const QJsonObject credentialWebSection = credentials["web"].toObject();

    for(auto jsonKey: credentialWebSection.keys())
    {
        if(oauth2Settings_.contains(jsonKey))
        {
            oauth2Settings_[jsonKey] = credentialWebSection[jsonKey];
        }
    }

    if(!saveSettings(oauth2Settings_["db_path"].toString(), oauth2Settings_))
    {
        qDebug() << "Failed to save json credentials to db" \
                 << "in OAuth2::setCredentialsFromJson";
        return false;
    }

    return true;
}

QJsonObject OAuth2::makeJsonFromFile(QString jsonCredentialPath)
{
    QByteArray credentials;
    QFile jsonCredentialFile(jsonCredentialPath);

    if(!jsonCredentialFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open json credential file in\
                    OAuth2::setOAuth2CredWithJson";
                    return QJsonObject();
    }

    while(!jsonCredentialFile.atEnd())
    {
        credentials.append(jsonCredentialFile.readLine());
    }

    jsonCredentialFile.close();
    return QJsonObject(QJsonDocument::fromJson(credentials).object());
}



QByteArray OAuth2::get()
{
    return QByteArray();
}
