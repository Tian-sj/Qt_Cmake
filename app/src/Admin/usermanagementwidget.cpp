#include <app/Admin/usermanagementwidget.h>

#include "ui_usermanagementwidget.h"

namespace Admin {

UserManagementWidget::UserManagementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserManagementWidget)
{
    ui->setupUi(this);
}

UserManagementWidget::~UserManagementWidget()
{
    delete ui;
}

}
