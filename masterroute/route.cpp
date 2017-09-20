#include "route.h"

Route::Route()
{
    startsPrevDay = QVector<bool>(7);
    regExpVector = {mrsSheetDateRegExp, routeRegExp,
                                         driverRegExp, equipmentRegExp};

}


int Route::whatIsThis(const QString &data)
{
    int matchIdx = -1;

    for(int k = 0; k < regExpVector.size(); k++)
    {
        matchIdx = -1;
        matchIdx = regExpVector.at(k).indexIn(data);
        if(matchIdx != -1)
        {
            switch(k)
            {
            case routeEnum::matchSheetDate:
                return routeEnum::matchSheetDate;
                break;

            case routeEnum::matchRouteKey:
                return routeEnum::matchRouteKey;
                break;

            case routeEnum::matchDriverName:
                return routeEnum::matchDriverName;
                break;

            case routeEnum::matchEquipment:
                return routeEnum::matchEquipment;
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
        date = QDateTime::fromString(data, dateFormat);
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

void Route::applyFullStartTimeOffset(RouteStartTime &rst)
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

        if(!date.isNull())
        {
            switch(dayOfWeek.value(date.toString("dddd")))
            {
            case routeEnum::mon:
                date = date.addMSecs(monMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::tue:
                date = date.addMSecs(tueMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::wed:
                date = date.addMSecs(wedMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::thu:
                date = date.addMSecs(thuMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::fri:
                date = date.addMSecs(friMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::sat:
                date = date.addMSecs(satMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            case routeEnum::sun:
                date = date.addMSecs(sunMidnightOffsetmSec);
                //qDebug() << key << date.toString();
                break;
            }
        }
    }
}

void Route::applySingleStartTimeOffset(qint64 &startTimeOffset, int dayOfWeek)
{

}

void Route::setDateFormat(const QString &qs)
{
    dateFormat = qs;
}

bool Route::isRouteValid()
{
    return true;
}

QDateTime Route::getRouteDate()
{
    return date;
}

QString Route::getKey()
{
    return key;
}

QString Route::getTruckNumber()
{
    return truckNum;
}

QString Route::getTrailerNumber()
{
    return trailerNum;
}

QString Route::getDriverName()
{
    return driverName;
}

QString Route::getDriverId()
{
    return driverId;
}

QString Route::getDateFormat()
{
    return dateFormat;
}

QStringList Route::getMisc()
{
    return misc;
}
