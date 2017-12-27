#include "route.h"

Route::Route()
{
    startsPrevDay = QVector<bool>(7);
    regExpVector = {mrsSheetDateRegExp,
                    routeRegExp,
                    driverRegExp,
                    equipmentRegExp,
                    timeRegExp};
}

int Route::whatIsThis(const QString &data)
{
    int matchIdx = -1;

    for(int k = 0; k < regExpVector.size(); k++)
    {
        matchIdx = -1;
        matchIdx = regExpVector.at(k).indexIn(data);

        if(matchIdx != -1 && (regExpVector.at(k).captureCount() == 0) && (regExpVector.at(k).cap().length() == data.length()))
        {
            switch(k)
            {
            case routeEnum::matchSheetDate:
                //qDebug() << "matchSheetDate" << data <<  "reg cap len == data len?" << (regExpVector.at(k).cap().length() == data.length());
                return routeEnum::matchSheetDate;
                break;

            case routeEnum::matchRouteKey:
               //qDebug() << "matchRouteKey" << data <<  "reg cap len == data len?" << (regExpVector.at(k).cap().length() == data.length());
                return routeEnum::matchRouteKey;
                break;

            case routeEnum::matchDriverName:
                //qDebug() <<  "matchDriverName" << data <<  "reg cap len == data len?" << (regExpVector.at(k).cap().length() == data.length());
                return routeEnum::matchDriverName;
                break;

            case routeEnum::matchEquipment:
                //qDebug() << "matchEquipment" << data <<  "reg cap len == data len?" << (regExpVector.at(k).cap().length() == data.length());
                return routeEnum::matchEquipment;
                break;

            case routeEnum::matchTime:
                //qDebug() << "matchTime" << data <<  "reg cap len == data len?" << (regExpVector.at(k).cap().length() == data.length());
                return routeEnum::matchTime;
                break;
            }
        }
    }
    if(data.trimmed().isEmpty())
        return routeEnum::matchEmpty;

    return routeEnum::matchMisc;
}

void Route::setField(const QString &data, int fieldEnum)
{
    switch(fieldEnum)
    {
    case routeEnum::key:
        key = data;
        break;

    case routeEnum::date:
        date = QDate::fromString(data, dateFormat);
        break;

    case routeEnum::baselineDepartureDate:
        baselineDeparture = QDateTime::fromString(data, dateFormat);
        break;

    case routeEnum::lateRouteOutTime:
        baselineDeparture = QDateTime::currentDateTime();
        baselineDeparture.setTime(QTime::fromString(data, timeFormat));
        break;

    case routeEnum::driverName:
        driverName = data;
        break;

    case routeEnum::driverId:
        driverId = data;
        break;

    case routeEnum::truckNum:
        truckNum = data;
        break;

    case routeEnum::trailerNum:
        trailerNum = data;
        break;

    case routeEnum::misc:
        misc.append(data);
        break;

    case routeEnum::empty:
        qDebug() << "Empty data, discarding";
        break;
    }
}

void Route::applyRouteBaselineDeparture(const RouteStartTime &rst)
{
    QMap<QString, int> dayOfWeek;
    dayOfWeek["Monday"]     = routeEnum::mon;
    dayOfWeek["Tuesday"]    = routeEnum::tue;
    dayOfWeek["Wednesday"]  = routeEnum::wed;
    dayOfWeek["Thursday"]   = routeEnum::thu;
    dayOfWeek["Friday"]     = routeEnum::fri;
    dayOfWeek["Saturday"]   = routeEnum::sat;
    dayOfWeek["Sunday"]     = routeEnum::sun;

    if(this->key == rst.routeKey)
    {
        startsPrevDay = rst.startsPrevDay;
        this->monMidnightOffsetmSec = rst.monMidnightOffsetmSec;
        this->tueMidnightOffsetmSec = rst.tueMidnightOffsetmSec;
        this->wedMidnightOffsetmSec = rst.wedMidnightOffsetmSec;
        this->thuMidnightOffsetmSec = rst.thuMidnightOffsetmSec;
        this->friMidnightOffsetmSec = rst.friMidnightOffsetmSec;
        this->satMidnightOffsetmSec = rst.satMidnightOffsetmSec;
        this->sunMidnightOffsetmSec = rst.sunMidnightOffsetmSec;

        if(!baselineDeparture.isNull())
        {
            switch(dayOfWeek.value(baselineDeparture.toString("dddd")))
            {
            case routeEnum::mon:
                baselineDeparture = baselineDeparture.addMSecs(monMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::tue:
                baselineDeparture = baselineDeparture.addMSecs(tueMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::wed:
                baselineDeparture = baselineDeparture.addMSecs(wedMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::thu:
                baselineDeparture = baselineDeparture.addMSecs(thuMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::fri:
                baselineDeparture = baselineDeparture.addMSecs(friMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::sat:
                baselineDeparture = baselineDeparture.addMSecs(satMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::sun:
                baselineDeparture = baselineDeparture.addMSecs(sunMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            }
        }
    }
}

void Route::setDateFormat(const QString &qs)
{
    dateFormat = qs;
}

bool Route::isRouteValid()
{
    return true;
}

QDateTime Route::getRouteBaselineDeparture() const
{
    return baselineDeparture;
}

QDate Route::getRouteDate() const
{
    return date;
}

QString Route::getKey() const
{
    return key;
}

QString Route::getTruckNumber() const
{
    return truckNum;
}

QString Route::getTrailerNumber() const
{
    return trailerNum;
}

QString Route::getDriverName() const
{
    return driverName;
}

QString Route::getDriverId() const
{
    return driverId;
}

QString Route::getDateFormat() const
{
    return dateFormat;
}

QStringList Route::getMisc() const
{
    return misc;
}
