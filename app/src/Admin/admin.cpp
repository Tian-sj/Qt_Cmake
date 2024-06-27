#include <app/Admin/admin.h>
#include <QPainter>

#include "ui_admin.h"

namespace Admin {

AdminWindow::AdminWindow(QWidget *parent) :
    MainWindow(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    initUI();
    initUIText();
}

AdminWindow::~AdminWindow()
{
    delete ui;
}

void AdminWindow::initUI()
{
    // 加载css样式
    loadCSS(":/css/AdminWindow.css");

    set_btn_max_visible(true);

    this->setAttribute(Qt::WA_TranslucentBackground);
}

void AdminWindow::initUIText()
{
}

void AdminWindow::set_btn_max_visible(bool visible)
{
    // NOT CHANGED
    if (visible) {
        ui->btn_max2->setVisible(false);
        ui->btn_max->setVisible(true);
    } else {
        ui->btn_max->setVisible(false);
        ui->btn_max2->setVisible(true);
    }
}

void AdminWindow::on_btn_max_clicked()
{
    set_btn_max_visible(false);
    this->showMaximized();
}

void AdminWindow::on_btn_max2_clicked()
{
    set_btn_max_visible(true);
    this->showNormal();
}



}
