#include <app/User/user.h>

#include "ui_user.h"

namespace User {

UserWindow::UserWindow(QWidget *parent) :
    MainWindow(parent),
    ui(new Ui::UserWindow)
{
    ui->setupUi(this);
}

UserWindow::~UserWindow()
{
    delete ui;
}

void UserWindow::initUI()
{

}

void UserWindow::initUIText()
{

}

}
