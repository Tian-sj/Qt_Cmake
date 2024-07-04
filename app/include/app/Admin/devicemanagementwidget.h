#ifndef DEVICEMANAGEMENTWIDGET_H
#define DEVICEMANAGEMENTWIDGET_H

#include <QWidget>

namespace Admin {

namespace Ui {
class DeviceManagementWidget;
}

class DeviceManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceManagementWidget(QWidget *parent = nullptr);
    ~DeviceManagementWidget();

private:
    Ui::DeviceManagementWidget *ui;
};

}

#endif // DEVICEMANAGEMENTWIDGET_H
