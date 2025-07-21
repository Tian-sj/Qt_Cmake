#include "gui/registration_expiration_reminder.hpp"
#include "gui/config.hpp"

#include "ui_registration_expiration_reminder.h"

RegistrationExpirationReminder::RegistrationExpirationReminder(bool *is_show, QWidget *parent)
    : BaseWindow<RegistrationExpirationReminder, Ui::RegistrationExpirationReminder, QDialog>(parent)
    , is_show_(is_show)
{
}

RegistrationExpirationReminder::~RegistrationExpirationReminder() {
}

void RegistrationExpirationReminder::initUi() {
    this->loadCSS(this, "");

    this->setWindowTitle(tr("Registration Code Expiration Reminder"));
    ui->lab_message->setWordWrap(true);
}

void RegistrationExpirationReminder::initUiText() {
    ui->retranslateUi(this);

    ui->lab_message->setText(tr("Your registration code will expired at %1 , please update the registration code as soon as possible, otherwise the expiration program will terminate automatically.").arg(Config::getInstance().getEndTime().toString("yyyy-MM-dd HH:mm:ss")));
    ui->check_box->setText(tr("Don't show this reminder again"));
    // 确认
    ui->btn_verify->setText(tr("Verify"));
}

void RegistrationExpirationReminder::initConnect() {

}


void RegistrationExpirationReminder::on_check_box_checkStateChanged(const Qt::CheckState &arg1) {
    *is_show_ = !arg1;
}

void RegistrationExpirationReminder::initLazy() {}
