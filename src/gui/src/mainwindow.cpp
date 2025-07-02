#include "gui/mainwindow.hpp"
#include "gui/registrationexpirationreminder.hpp"

#include <QVBoxLayout>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QTextLayout>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , timer_rc(new QTimer(this))
    , is_show(true)
    , is_current_show(false)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init_ui();
    init_ui_text();
    init_connect();
}

MainWindow::~MainWindow()
{
}

void MainWindow::init_ui() {

    loadCSS(this, Config::getInstance().getThemePath() + "gui-themes.css");

    timer_rc->setInterval(60000);
    timer_rc->start();
}

void MainWindow::init_connect(){
    connect_language_changed(this);
    connect(timer_rc, &QTimer::timeout, this, &MainWindow::on_timer_rc);
}

void MainWindow::init_ui_text(){
    ui->retranslateUi(this);
    this->setWindowTitle(tr(_NAME));
}

void MainWindow::on_timer_rc()
{
    if (is_show && !is_current_show && Config::getInstance().checkExpirationReminder()) {
        RegistrationExpirationReminder *rer = new RegistrationExpirationReminder(&is_show);
        is_current_show = true;
        rer->exec();
        is_current_show = false;
        delete rer;
        rer = nullptr;
    }

    if (QDateTime::currentDateTime() < Config::getInstance().getAppRuntime()) {
        QMessageBox::critical(nullptr, "Error", "The application has expired, please contact the vendor for a new license key.");
        qApp->exit();
    }

    if (QDateTime::currentDateTime() > Config::getInstance().getAppRuntime()) {
        Config::getInstance().setAppRuntime(QDateTime::currentDateTime());
    }

    if (QDateTime::currentDateTime() >= Config::getInstance().getEndTime()) {
        qApp->exit();
    }
}
