#include "route.h"

Route::Route()
{

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

void Route::setDateFormat(const QString &qs)
{
    dateFormat = qs;
}

bool Route::isRouteValid()
{

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
