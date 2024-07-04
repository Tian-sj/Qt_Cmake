#include <app/Admin/deviceviewwidget.h>
#include <QFile>

#include "ui_deviceviewwidget.h"

namespace Admin {

DeviceViewWidget::DeviceViewWidget(QWidget *parent)
    : QWidget(parent)
    , is_DVW_btn_show_hide_state(false)
    , ui(new Ui::DeviceViewWidget)
{
    ui->setupUi(this);

    initUI();
    initUIText();
    initConnect();
}

DeviceViewWidget::~DeviceViewWidget()
{
    delete ui;
}

void DeviceViewWidget::initUI()
{
    loadCSS(":/css/Admin/DeviceViewWidget.css");
    set_DVW_btn_show_hide_state(is_DVW_btn_show_hide_state);
    model = ui->DVW_tree_item->createModel();
    ui->DVW_tree_item->setModel(model);
}

void DeviceViewWidget::initUIText()
{

}

void DeviceViewWidget::initConnect()
{
}

void DeviceViewWidget::loadCSS(const QString &cssFile, QWidget* w)
{
    QFile file(cssFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    QTextStream stream(&file);
    QString styleSheet = stream.readAll();
    if (!w)
        this->setStyleSheet(styleSheet);
    else
        w->setStyleSheet(styleSheet);

    file.close();
}

void DeviceViewWidget::set_DVW_btn_show_hide_state(bool state)
{
    if (state) {
        ui->DVW_tree_item->setFixedWidth(0);
        loadCSS(":/css/Admin/DeviceViewWidget/btn-show.css", ui->DVW_btn_show_hide);
    }
    else {
        ui->DVW_tree_item->setFixedWidth(230);
        loadCSS(":/css/Admin/DeviceViewWidget/btn-hide.css", ui->DVW_btn_show_hide);
    }
}

void DeviceViewWidget::on_DVW_btn_show_hide_clicked()
{
    is_DVW_btn_show_hide_state = !is_DVW_btn_show_hide_state;
    set_DVW_btn_show_hide_state(is_DVW_btn_show_hide_state);
}

}
