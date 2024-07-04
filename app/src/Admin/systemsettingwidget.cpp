#include <app/Admin/systemsettingwidget.h>

#include "ui_systemsettingwidget.h"

namespace Admin {

SystemSettingWidget::SystemSettingWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemSettingWidget)
{
    ui->setupUi(this);
}

SystemSettingWidget::~SystemSettingWidget()
{
    delete ui;
}

}
