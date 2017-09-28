#ifndef DAILYLATEGREENMILEWIDGET_H
#define DAILYLATEGREENMILEWIDGET_H

#include <QWidget>

namespace Ui {
class DailyLateGreenmileWidget;
}

class DailyLateGreenmileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DailyLateGreenmileWidget(QWidget *parent = 0);
    ~DailyLateGreenmileWidget();

private:
    Ui::DailyLateGreenmileWidget *ui;
};

#endif // DAILYLATEGREENMILEWIDGET_H
