#ifndef ROUTESTARTTIME_H
#define ROUTESTARTTIME_H

#include <QtCore>

struct RouteStartTime
{
    enum {mon, tue, wed, thu, fri, sat, sun};
    bool startsPrevDay [7];
    QString     routeKey;
    //The days a route starts on a prev day vector
    //0 is sunday;

    qint64 monMidnightOffsetSec;
    qint64 tueMidnightOffsetSec;
    qint64 wedMidnightOffsetSec;
    qint64 thuMidnightOffsetSec;
    qint64 friMidnightOffsetSec;
    qint64 satMidnightOffsetSec;
    qint64 sunMidnightOffsetSec;
}
#endif // ROUTESTARTTIME_H
