#include "routedifference.h"

RouteDifference::RouteDifference()
{

}

bool RouteDifference::operator==(const RouteDifference &rhs) const
{
    if(this->hasDiscrepencies       == rhs.hasDiscrepencies     &&
       this->routeExistsInSourceA   == rhs.routeExistsInSourceA &&
       this->routeExistsInSourceB   == rhs.routeExistsInSourceB &&
       this->driverMismatch         == rhs.driverMismatch       &&
       this->truckMismatch          == rhs.truckMismatch        &&
       this->routeKey               == rhs.routeKey             &&
       this->powerUnitA             == rhs.powerUnitA           &&
       this->driverNameA            == rhs.driverNameA          &&
       this->driverNumberA          == rhs.driverNumberA        &&
       this->powerUnitB             == rhs.powerUnitB           &&
       this->driverNameB            == rhs.driverNameB          &&
       this->driverNumberB          == rhs.driverNumberB)
    {
       return true;
    }
    else
        return false;
}

 RouteDifference RouteDifference::findDifference
(const Route &routeA, const Route &routeB)
{
    RouteDifference difference;

    difference.routeKey = routeA.getKey();

    difference.powerUnitA = routeA.getTruckNumber();
    difference.driverNameA = routeA.getDriverName();
    difference.driverNumberA = routeA.getDriverId();

    difference.powerUnitB = routeB.getTruckNumber();
    difference.driverNameB = routeB.getDriverName();
    difference.driverNumberB = routeB.getDriverId();

    if(difference.powerUnitA != difference.powerUnitB)
    {
        difference.hasDiscrepencies = true;
        difference.truckMismatch = true;
    }

    if(difference.driverNumberA != difference.driverNumberB)
    {
        difference.hasDiscrepencies = true;
        difference.driverMismatch = true;
    }

    return difference;
}

 QVector<RouteDifference> RouteDifference::findDifferences
(const QVector<Route> &routesA,
 const QVector<Route> &routesB)
{
    QVector<RouteDifference> differences;

    differences.append(compareVectorAtoB(routesA, routesB));

    std::sort(differences.begin(), differences.end(),
            [](RouteDifference rd1, RouteDifference rd2) ->\
            bool {return rd1.routeKey < rd2.routeKey;});

    QVector<RouteDifference> reeee;
    for(auto diff: differences)
    {
        if(diff.routeKey == "J-WXX")
        {
            reeee.append(diff);
            diff.printDebug();
        }

    }

    for(auto diff: reeee)
    {
        RouteDifference difft = diff;
        for(auto diff2:reeee)
        {
            if((difft == diff2) && (reeee.indexOf(difft) != reeee.indexOf(diff2)))
            {
                difft.printDebug();
                diff2.printDebug();
            }
        }
    }

    differences.erase(std::unique(differences.begin(), differences.end()), differences.end());



    for(auto diff : differences)
    {
        auto it = differences.begin();
        int occurrences = 0;
        while(it != differences.end())
        {
            //qDebug() << diff.routeKey << occurrences << "b4" << int(it - differences.begin());
            it = std::find_if(it, differences.end(), [&diff](const RouteDifference &vecDiff){return vecDiff.routeKey == diff.routeKey;});
            if(it != differences.end())
            {
                occurrences++;
                if(occurrences > 1)
                {
                    it->routeKey = it->routeKey.append(QString("_" + QString::number(occurrences)));
                    qDebug() << it->routeKey << occurrences << "aftr";
                    it++;
                }
                else
                    it++;
            }

        }
    }

    for(auto it = differences.begin(); it != differences.end();)
    {
        if(!it->hasDiscrepencies)
        {
            it = differences.erase(it);
        }
        else
            ++it;
    }

    for(auto diff: differences)
    {
        qDebug() << diff.routeKey;
    }
    return differences;
}

void RouteDifference::printDebug() const
{
    qDebug() << "---------------";
    qDebug() << "routeKey" << this->routeKey;
    qDebug() << "hasDiscrepencies" << this->hasDiscrepencies;
    qDebug() << "routeExistsInSourceA" << this->routeExistsInSourceA;
    qDebug() << "routeExistsInSourceB" << this->routeExistsInSourceB;
    qDebug() << "driverMismatch" << this->driverMismatch;
    qDebug() << "truckMismatch" << this->truckMismatch;
    qDebug() << "powerUnitA" << this->powerUnitA;
    qDebug() << "driverNameA" << this->driverNameA;
    qDebug() << "driverNumberA" << this->driverNumberA;
    qDebug() << "powerUnitB" << this->powerUnitB;
    qDebug() << "driverNameB" << this->driverNameB;
    qDebug() << "driverNumberB" << this->driverNumberB;
    qDebug() << "---------------";
}

 QVector<RouteDifference> RouteDifference::compareVectorAtoB
(const QVector<Route> routesA, const QVector<Route> routesB)
{
    RouteDifference difference;
    QVector<RouteDifference> differences;

    for(auto routeA: routesA)
    {
        if(routesB.contains(routeA))
        {
            Route routeB = routesB.at(routesB.indexOf(routeA));
            difference = findDifference(routeA, routeB);
            differences.append(difference);
        }
        else
        {
            difference = onlyExistsInA(routeA);
            differences.append(difference);
        }
    }

    return differences;
}

 QVector<RouteDifference> RouteDifference::compareVectorBtoA
(const QVector<Route> routesB, const QVector<Route> routesA)
{
    RouteDifference difference;
    QVector<RouteDifference> differences;

    for(auto routeB: routesB)
    {
        if(routesA.contains(routeB))
        {
            Route routeA = routesA.at(routesA.indexOf(routeB));
            difference = findDifference(routeA, routeB);
            differences.append(difference);
        }
        else
        {
            difference = onlyExistsInB(routeB);
            differences.append(difference);
        }
    }

    return differences;
}

 RouteDifference RouteDifference::onlyExistsInA(const Route &routeA)
{
    RouteDifference difference;
    difference.routeKey = routeA.getKey();
    difference.hasDiscrepencies = true;
    difference.routeExistsInSourceA = true;
    difference.routeExistsInSourceB = false;

    if(!routeA.getDriverId().isEmpty())
        difference.driverMismatch = true;

    if(!routeA.getTruckNumber().isEmpty())
        difference.truckMismatch = true;

    difference.powerUnitA = routeA.getTruckNumber();
    difference.driverNameA = routeA.getDriverName();
    difference.driverNumberA = routeA.getDriverId();

    return difference;
}

 RouteDifference RouteDifference::onlyExistsInB(const Route &routeB)
{
    RouteDifference difference;
    difference.routeKey = routeB.getKey();
    difference.hasDiscrepencies = true;
    difference.routeExistsInSourceA = false;
    difference.routeExistsInSourceB = true;

    if(!routeB.getDriverId().isEmpty())
        difference.driverMismatch = true;

    if(!routeB.getTruckNumber().isEmpty())
        difference.truckMismatch = true;

    difference.powerUnitB = routeB.getTruckNumber();
    difference.driverNameB = routeB.getDriverName();
    difference.driverNumberB = routeB.getDriverId();

    return difference;
}
