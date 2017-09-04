#include "netconnect.h"

NetConnect::NetConnect(QObject *parent) : QObject(parent)
{

}


NetConnect::~NetConnect()
{
}

QByteArray NetConnect::postRequest(const QStringList &headers, const QString &address, const QString &body)
{
    qDebug() << address;

    //Format and attach url to network request.
    QUrl url(address.toLatin1());
    QNetworkRequest request(url);
    QByteArray response;
    QString errorStr;

    //For each header, split on a : and attach to
    //the request.
    for(auto h : headers)
    {
        QStringList chunks = h.split(":");
        //Headers should only split into two chunks.
        if(chunks.length() == 2)
        {
            //serialize headers
            request.setRawHeader(QByteArray(chunks.value(0).toUtf8()), QByteArray(chunks.value(1).toUtf8()));
        }
        //If headers do not split into two chunks, complain and return empty QBtyeArray
        else
        {
            errorStr =  "A header did not cleanly split into two parts,"
                        " please verify integrity of your headers.";
            emit networkError(errorStr);
            return QByteArray();
        }
    }

    //Create a network access manager.
    QNetworkAccessManager nam;

    //Send a post request with the network manager. nam.post returns a pointer. I find this to be an
    //inconvenience and delete the post at the end of this function.
    QNetworkReply *reply = nam.post(request, QJsonDocument(QJsonDocument::fromJson(body.toUtf8())).toJson());

    //While waiting on the server do not lock up the ui on main thread.
    while(!reply->isFinished())
    {
        qApp->processEvents();
    }

    //If all went well, turn the reply into a bytearray.
    if (reply->error() == QNetworkReply::NoError) {
        response = reply->readAll();
    }

    // an error occurred. emit a signal with the error message
    // to any function that cares.
    else {
        errorStr = "Error: " + reply->errorString();
        qDebug() << errorStr;
        emit networkError(errorStr);
        //QMessageBox::information(this, "", stringReply);
    }

    reply->deleteLater();
    return response;
}
