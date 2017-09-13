#include "route.h"


Route::Route()
{

}

//Route::Route(Route &other)
//{

//}

//Route::~Route()
//{

//}

int Route::whatIsThis(const QString &data)
{
    int matchIdx = -1;
    QString dateFormat = "d-MMM-yyyy";

    bool foundDate = false;
    bool foundMatch = false;


    for(int k = 0; k < regExpVector.size(); k++)
    {
        matchIdx = -1;
        matchIdx = regExpVector.at(k)->indexIn(data);
        if(matchIdx != -1)
        {
            switch(k)
            {
            case matchSheetDate:
                return matchSheetDate;
                break;

            case matchRouteKey:
                return matchRouteKey;
                break;

            case matchDriverName:
                return matchDriverName;
                break;

            case matchEquipment:
                return matchEquipment;
                break;
            }
        }
    }
    if(data.trimmed().isEmpty())
        return matchEmpty;

    return matchMisc;
}

void Route::setField(const QString &data, int fieldEnum)
{
    switch(fieldEnum)
    {
    case mKey:
        key = data;
        break;

    case mDate:
        date = QDateTime::fromString(data, dateFormat);
        break;

    case mDriverName:
        driverName = data;
        break;

    case mDriverId:
        driverId = data;
        break;

    case mTruckNum:
        truckNum = data;
        break;

    case mTrailerNum:
        trailerNum = data;
        break;

    case mMisc:
        misc.append(data);
        break;

    case mEmpty:
        qDebug() << "Empty data, discarding";
        break;
    }
}

bool Route::build()
{

}

void Route::setDateFormat(const QString &qs)
{
    dateFormat = qs;
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

QStringList Route::getMisc()
{
    return misc;
}

bool Route::isRouteValid()
{

}
