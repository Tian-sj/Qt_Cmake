#ifndef SYSTEMSETTINGWIDGET_H
#define SYSTEMSETTINGWIDGET_H

#include <QWidget>

namespace Admin {

namespace Ui {
class SystemSettingWidget;
}

class SystemSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSettingWidget(QWidget *parent = nullptr);
    ~SystemSettingWidget();

private:
    Ui::SystemSettingWidget *ui;
};

}

#endif // SYSTEMSETTINGWIDGET_H
