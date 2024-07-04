#include <app/Admin/admin.h>
#include <app/settingwidget.h>
#include <QPainter>
#include <QListView>
#include <QResizeEvent>
#include <QVariant>
#include <QRect>

#include "ui_admin.h"

namespace Admin {

AdminWindow::AdminWindow(QWidget *parent)
    : MainWindow(parent)
    , is_show_setting_widget(false)
    , DVW(new DeviceViewWidget(this))
    , DMW(new DeviceManagementWidget(this))
    , UMW(new UserManagementWidget(this))
    , SSW(new SystemSettingWidget(this))
    , ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    initUI();
    initUIText();
    initConnect();
}

AdminWindow::~AdminWindow()
{
    if (act1_change_user) {
        delete act1_change_user;
        act1_change_user = nullptr;
    }

    if (act2_change_user) {
        delete act2_change_user;
        act2_change_user = nullptr;
    }

    if (menu_change_user) {
        delete menu_change_user;
        menu_change_user = nullptr;
    }

    delete ui;
}

void AdminWindow::initUI()
{
    loadCSS(":/css/Admin/AdminWindow.css");

    set_btn_max_visible(true);

    ui->WINDOWS->addWidget(qobject_cast<QWidget *>(DVW));
    ui->WINDOWS->addWidget(qobject_cast<QWidget *>(DMW));
    ui->WINDOWS->addWidget(qobject_cast<QWidget *>(UMW));
    ui->WINDOWS->addWidget(qobject_cast<QWidget *>(SSW));

    menu_change_user = new QMenu(this);
    act1_change_user = new QAction("", this);
    act2_change_user = new QAction("", this);

    menu_change_user->setObjectName("menu_change_user");

    menu_change_user->addAction(act1_change_user);
    menu_change_user->addAction(act2_change_user);

    ui->btn_change_user->setMenu(menu_change_user);

    this->setAttribute(Qt::WA_TranslucentBackground);

    QButtonGroup* btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->btn1);
    btnGroup->addButton(ui->btn2);
    btnGroup->addButton(ui->btn3);
    btnGroup->addButton(ui->btn4);
    btnGroup->addButton(ui->btn5);
    btnGroup->addButton(ui->btn6);

    btn_group_map[ui->btn1->objectName()] = BtnGroupType::DeviceView;
    btn_group_map[ui->btn2->objectName()] = BtnGroupType::DataAnalysis;
    btn_group_map[ui->btn3->objectName()] = BtnGroupType::DeviceManagement;
    btn_group_map[ui->btn4->objectName()] = BtnGroupType::LogManagement;
    btn_group_map[ui->btn5->objectName()] = BtnGroupType::UserManagement;
    btn_group_map[ui->btn6->objectName()] = BtnGroupType::SystemSetting; 

    QList<QAbstractButton *> btns = btnGroup->buttons();
    for (QAbstractButton *btn : btns) {
        QToolButton *b = static_cast<QToolButton *>(btn);
        connect(b, &QAbstractButton::clicked, this, &AdminWindow::on_gtoup_list_btn_clicked);
        b->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        if (btn_group_map[b->objectName()] == BtnGroupType::DataAnalysis || btn_group_map[b->objectName()] == BtnGroupType::LogManagement)
            continue;
        b->setCheckable(true);
    }

    ui->btn1->setChecked(true);

    set_widget = new SettingWidget(this);
    set_widget->setObjectName("SettingWidget");
    set_widget->raise();
    set_widget->setGeometry(0, 0, 0, 0);

    animation_start = new QPropertyAnimation(set_widget, "geometry");
    animation_start->setDuration(300);

    animation_stop = new QPropertyAnimation(set_widget, "geometry");
    animation_stop->setDuration(300);
}

void AdminWindow::initUIText()
{
    ui->btn_change_user->setText(tr("Administrator"));
    act1_change_user->setText(tr("Change User"));
    act2_change_user->setText(tr("Exit"));

    ui->btn1->setText(tr("Device View"));
    ui->btn2->setText(tr("Data Analysis"));
    ui->btn3->setText(tr("Device Management"));
    ui->btn4->setText(tr("Log Management"));
    ui->btn5->setText(tr("User Management"));
    ui->btn6->setText(tr("System Setting"));
}

void AdminWindow::initConnect()
{
    connect(act1_change_user, &QAction::triggered, this, &AdminWindow::on_act1_change_user_clicked);
    connect(act2_change_user, &QAction::triggered, this, &AdminWindow::on_act2_change_user_clicked);
    connect(set_widget, &SettingWidget::_close, this, &AdminWindow::on_setting_widget_close_clicked);
}

void AdminWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateChildWindowGeometry();
}

void AdminWindow::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    updateChildWindowGeometry();
}

void AdminWindow::mousePressEvent(QMouseEvent *event)
{
    if (is_show_setting_widget && !set_widget->geometry().contains(event->pos())) {
        on_setting_widget_close_clicked();
    }
    QWidget::mousePressEvent(event);
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

void AdminWindow::updateChildWindowGeometry()
{
    animation_start->setStartValue(QRect(this->width(), 0, 0, this->height()));
    animation_start->setEndValue(QRect(this->width() - set_widget->getWidth(), 0, set_widget->getWidth(), this->height()));
    
    animation_stop->setStartValue(QRect(this->width() - set_widget->getWidth(), 0, set_widget->getWidth(), this->height()));
    animation_stop->setEndValue(QRect(this->width(), 0, 0, this->height()));

    if (is_show_setting_widget)
        set_widget->setGeometry(this->width() - set_widget->getWidth(), 0, set_widget->getWidth(), this->height());
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

void AdminWindow::on_btn_change_user_clicked(bool checked)
{

    QPoint pos = ui->btn_change_user->mapToGlobal(QPoint(0, ui->btn_change_user->height()));
    menu_change_user->exec(pos);
}

void AdminWindow::on_act1_change_user_clicked()
{
    qApp->exit(100);
}

void AdminWindow::on_act2_change_user_clicked()
{
    this->close();
}

void AdminWindow::on_gtoup_list_btn_clicked()
{
    QAbstractButton *btn = qobject_cast<QAbstractButton *>(sender());

    switch (btn_group_map[btn->objectName()]) {
        case BtnGroupType::DeviceView: {
            ui->WINDOWS->setCurrentWidget(DVW);
            break;
        }
        case BtnGroupType::DataAnalysis: {
            DataAnalysisWidget* DAW = new DataAnalysisWidget;
            DAW->show();
            break;
        }
        case BtnGroupType::DeviceManagement: {
            ui->WINDOWS->setCurrentWidget(DMW);
            break;
        }
        case BtnGroupType::LogManagement: {
            LogManagementWidget* LMW = new LogManagementWidget;
            LMW->show();
            break;
        }
        case BtnGroupType::UserManagement: {
            ui->WINDOWS->setCurrentWidget(UMW);
            break;
        }
        case BtnGroupType::SystemSetting: {
            ui->WINDOWS->setCurrentWidget(SSW);
            break;
        }
        default:
            break;
    }
}

void AdminWindow::on_btn_setting_clicked() {
    is_show_setting_widget = true;
    animation_start->start();
}

void AdminWindow::on_setting_widget_close_clicked()
{
    is_show_setting_widget = false;
    animation_stop->start();
}

void AdminWindow::set_test() {

}

}
