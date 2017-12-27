#ifndef ROUTE_H
#define ROUTE_H

#include <QRegExp>
#include <QtCore>

namespace routeEnum
{
    enum {key, truckNum, trailerNum, driverName, driverId, date, timeInOffice, timeOutOffice, lateRouteOutTime, misc, empty, baselineDepartureDate};
    enum {matchSheetDate, matchRouteKey, matchDriverName, matchEquipment, matchTime, matchMisc, matchEmpty};
    enum {mon, tue, wed, thu, fri, sat, sun};
}
#include "routestarttime.h"

class Route
{
public:
    Route();
    bool operator==(const Route &rhs) const {return this->getKey() == rhs.getKey();}
    int  whatIsThis(const QString &data);
    void setField(const QString &data, int fieldEnum);
    void applyRouteBaselineDeparture(const RouteStartTime &rst);
    void setDateFormat(const QString &qs);
    bool isRouteValid();
    QDateTime getRouteBaselineDeparture() const;
    QDate getRouteDate() const;
    QString getKey() const;
    QString getTruckNumber() const;
    QString getTrailerNumber() const;
    QString getDriverName() const;
    QString getDriverId() const;
    QString getDateFormat() const;
    QStringList getMisc() const;

private:
    //-------------------------------------------------------------------------
    // Regex used for adding fields to the class.
    // The incoming data is validated against this vector of regex.
    // Once the data is put into one of the categories,
    // key,
    //-------------------------------------------------------------------------
    QRegExp mrsSheetDateRegExp  = QRegExp("\\d+-[A-Z,a-z]{3}-\\d+");
    QRegExp routeRegExp         = QRegExp("^[A-Z]-[A-Z,0-9]{3}");
    QRegExp driverRegExp        = QRegExp("(?:\\w+-)?\\w+'?\\w+,\\s[A-Z]");
    QRegExp equipmentRegExp     = QRegExp("(?:\\b\\d+[a-z,A-Z]{1}\\b)|\\b\\d+\\b");
    QRegExp timeRegExp          = QRegExp("\\d+:\\d+|\\d{3,4}");

    //Added all regexp to vector allow for more succinct code.
    QVector<QRegExp> regExpVector;

    QString dateFormat = "d-MMM-yyyy";
    QString timeFormat = "h:mm";

    QDateTime   baselineDeparture;
    QDate       date;
    QString     key;
    QString     truckNum;
    QString     trailerNum;
    QString     driverName;
    QString     driverId;
    QStringList misc;

    //std::array<bool, 7> startsPrevDay;
    QVector<bool> startsPrevDay;
    qint64 monMidnightOffsetmSec;
    qint64 tueMidnightOffsetmSec;
    qint64 wedMidnightOffsetmSec;
    qint64 thuMidnightOffsetmSec;
    qint64 friMidnightOffsetmSec;
    qint64 satMidnightOffsetmSec;
    qint64 sunMidnightOffsetmSec;
};

#endif // ROUTE_H
