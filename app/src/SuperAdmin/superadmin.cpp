#include <app/SuperAdmin/superadmin.h>

#include "ui_superadmin.h"

namespace SurperAdmin {

SurperAdminWindow::SurperAdminWindow(QWidget *parent) :
    MainWindow(parent),
    ui(new Ui::SurperAdminWindow)
{
    ui->setupUi(this);
}

SurperAdminWindow::~SurperAdminWindow()
{
    delete ui;
}

void SurperAdminWindow::initUI()
{

}

void SurperAdminWindow::initUIText()
{

}

}
