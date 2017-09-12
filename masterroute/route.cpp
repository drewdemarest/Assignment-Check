#include "route.h"


Route::Route()
{

}

Route::Route(Route &other)
{

}

Route::~Route()
{

}

bool Route::addField(QString &data)
{
    for(int k = 0; k < regExpVector.size(); k++)
    {
        if(foundMatch)
            break;

        if(foundDate && k == matchSheetDate)
            continue;

        //qDebug() << k;
        matchIdx = -1;
        matchIdx = regExpVector.at(k)->indexIn(routeTuple.at(col).toString());
        if(matchIdx != -1)
        {
            switch(k)
            {
            case matchSheetDate:
                sheetDate = QDate::fromString(mrsSheetDateRegExp.cap(matchIdx), dateFormat);
                foundDate = true;
                foundMatch = true;
                break;

            case matchRoute:
                routeKeyFound.append(col);
                foundMatch = true;
                break;

            case matchDriver:
                driverFound.append(col);
                foundMatch = true;
                break;

            case matchEquipment:
                equipmentFound.append(col);
                foundMatch = true;
                break;
            }
        }
}

bool Route::build()
{

}

bool Route::isValid()
{

}
