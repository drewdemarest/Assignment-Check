#ifndef MASTERROUTEWIDGET_H
#define MASTERROUTEWIDGET_H

#include <QWidget>

namespace Ui {
class MasterRouteWidget;
}

class MasterRouteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MasterRouteWidget(QWidget *parent = 0);
    ~MasterRouteWidget();

private:
    Ui::MasterRouteWidget *ui;
};

#endif // MASTERROUTEWIDGET_H
