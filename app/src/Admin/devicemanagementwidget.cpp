#include <app/Admin/devicemanagementwidget.h>

#include "ui_devicemanagementwidget.h"

namespace Admin {

DeviceManagementWidget::DeviceManagementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DeviceManagementWidget)
{
    ui->setupUi(this);
}

DeviceManagementWidget::~DeviceManagementWidget()
{
    delete ui;
}

}
