#ifndef GREENMILE_H
#define GREENMILE_H

#include <QObject>

class Greenmile : public QObject
{
    Q_OBJECT
public:
    explicit Greenmile(QObject *parent = nullptr);

signals:

public slots:
};

#endif // GREENMILE_H