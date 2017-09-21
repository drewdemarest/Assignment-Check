#ifndef GREENMILEWIDGET_H
#define GREENMILEWIDGET_H

#include <QWidget>

namespace Ui {
class GreenmileWidget;
}

class GreenmileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GreenmileWidget(QWidget *parent = 0);
    ~GreenmileWidget();

private:
    Ui::GreenmileWidget *ui;
};

#endif // GREENMILEWIDGET_H
