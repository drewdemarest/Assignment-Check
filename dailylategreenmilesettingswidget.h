#ifndef DAILYLATEGREENMILESETTINGSWIDGET_H
#define DAILYLATEGREENMILESETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class DailyLateGreenmileSettingsWidget;
}

class DailyLateGreenmileSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DailyLateGreenmileSettingsWidget(QWidget *parent = 0);
    ~DailyLateGreenmileSettingsWidget();

private:
    Ui::DailyLateGreenmileSettingsWidget *ui;
};

#endif // DAILYLATEGREENMILESETTINGSWIDGET_H
