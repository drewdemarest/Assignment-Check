#ifndef ROUTE_H
#define ROUTE_H

#include <QRegExp>
#include <QtCore>

class Route
{
public:
    Route();
    Route(Route &other);
    ~Route();

    bool addField(QString &data);
    bool build();
    bool isValid();

private:

    //-------------------------------------------------------------------------
    // Regex used for adding fields to the class.
    // The incoming data is validated against this vector of regex.
    // Once the data is put into one of the categories,
    // key,
    //-------------------------------------------------------------------------
    enum {matchSheetDate, matchRoute, matchDriver, matchEquipment, matchTime};
    QRegExp mrsSheetDateRegExp  = QRegExp("\\d+-[A-Z,a-z]{3}-\\d+");
    QRegExp routeRegExp         = QRegExp("^[A-Z]-[A-Z,0-9]{3}");
    QRegExp driverRegExp        = QRegExp("(\\w')?(\\w+),\\s[A-Z]");
    QRegExp equipmentRegExp     = QRegExp("\\b\\d+\\b");
    QRegExp timeRegExp          = QRegExp("\\d+:\\d+)?(\\d){4,3}");
    //Added all regexp to vector allow for more succinct code.
    const QVector<QRegExp*> regExpVector = {&mrsSheetDateRegExp, &routeRegExp,
                                     &driverRegExp, &equipmentRegExp};

    QString     routeKey;
    QString     truckNumber;
    QString     trailerNumber;
    QString     driverName;
    QString     driverNumber;

    QStringList miscData;
    QDateTime   baselineDeparture;
};

#endif // ROUTE_H
