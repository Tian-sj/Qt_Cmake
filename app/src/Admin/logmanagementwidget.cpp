#include <app/Admin/logmanagementwidget.h>

#include "ui_logmanagementwidget.h"

namespace Admin {

LogManagementWidget::LogManagementWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogManagementWidget)
{
    ui->setupUi(this);

    initUI();
    initUIText();
    initConnect();
}

LogManagementWidget::~LogManagementWidget()
{
    delete ui;
}

void LogManagementWidget::initUI()
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void LogManagementWidget::initUIText()
{

}

void LogManagementWidget::initConnect()
{

}

}
