#ifndef ROUTE_H
#define ROUTE_H

#include <QRegExp>
#include <QtCore>

class Route
{
public:
    enum {mKey, mTruckNum, mTrailerNum, mDriverName, mDriverId, mDate, mMisc, mEmpty};
    enum {matchSheetDate, matchRouteKey, matchDriverName, matchEquipment, matchTime, matchMisc, matchEmpty};
    Route();
    //Route(Route &other);
    //~Route();

    int  whatIsThis(const QString &data);
    bool isRouteValid();
    bool build();

    void setField(const QString &data, int fieldEnum);
    void setDateFormat(const QString &qs);
    QDateTime getRouteDate();
    QString getKey();
    QString getTruckNumber();
    QString getTrailerNumber();
    QString getDriverName();
    QString getDriverId();
    QStringList getMisc();

private:
    //-------------------------------------------------------------------------
    // Regex used for adding fields to the class.
    // The incoming data is validated against this vector of regex.
    // Once the data is put into one of the categories,
    // key,
    //-------------------------------------------------------------------------
    QRegExp mrsSheetDateRegExp  = QRegExp("\\d+-[A-Z,a-z]{3}-\\d+");
    QRegExp routeRegExp         = QRegExp("^[A-Z]-[A-Z,0-9]{3}");
    QRegExp driverRegExp        = QRegExp("(\\w')?(\\w+),\\s[A-Z]");
    QRegExp equipmentRegExp     = QRegExp("\\b\\d+\\b");
    QRegExp timeRegExp          = QRegExp("\\d+:\\d+)?(\\d){4,3}");

    //Added all regexp to vector allow for more succinct code.
    QVector<QRegExp> regExpVector = {mrsSheetDateRegExp, routeRegExp,
                                     driverRegExp, equipmentRegExp};

    QString dateFormat = "d-MMM-yyyy";

    QDateTime   date;
    QString     key;
    QString     truckNum;
    QString     trailerNum;
    QString     driverName;
    QString     driverId;
    QStringList misc;
};

#endif // ROUTE_H
