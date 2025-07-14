#include "gui/main_window.hpp"
#include "gui/registration_expiration_reminder.hpp"
#include "gui/config.hpp"

#include <QVBoxLayout>
#include <QDateTime>
#include <QMessageBox>
#include <QTextLayout>

#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : BaseWindow<MainWindow, Ui::MainWindow, QMainWindow>(parent)
    , timer_rc_(new QTimer(this))
    , is_show_(true)
    , is_current_show_(false)
{
}

MainWindow::~MainWindow() {
}

void MainWindow::initUi() {

    loadCSS(this, Config::getInstance().getThemePath() + "gui-themes.css");

    timer_rc_->setInterval(60000);
    timer_rc_->start();
}

void MainWindow::initConnect(){
    connect(timer_rc_, &QTimer::timeout, this, &MainWindow::timerRc);
}

void MainWindow::initUiText(){
    ui->retranslateUi(this);
    this->setWindowTitle(tr(_NAME));
}

void MainWindow::timerRc() {
    if (is_show_ && !is_current_show_ && Config::getInstance().checkExpirationReminder()) {
        RegistrationExpirationReminder *rer = new RegistrationExpirationReminder(&is_show_);
        is_current_show_ = true;
        rer->exec();
        is_current_show_ = false;
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
