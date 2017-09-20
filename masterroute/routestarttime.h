#ifndef ROUTESTARTTIME_H
#define ROUTESTARTTIME_H

#include <QtCore>

struct RouteStartTime
{
    enum {mon, tue, wed, thu, fri, sat, sun};

    //std::array<bool, 7> startsPrevDay;
    QVector<bool> startsPrevDay = QVector<bool>(7);
    QString     routeKey;
    //The days a route starts on a prev day vector
    //0 is sunday;

    qint64 monMidnightOffsetmSec;
    qint64 tueMidnightOffsetmSec;
    qint64 wedMidnightOffsetmSec;
    qint64 thuMidnightOffsetmSec;
    qint64 friMidnightOffsetmSec;
    qint64 satMidnightOffsetmSec;
    qint64 sunMidnightOffsetmSec;
};
#endif // ROUTESTARTTIME_H
