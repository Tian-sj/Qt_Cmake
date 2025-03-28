#include "gui/registrationexpirationreminder.h"
#include "gui/config.h"

#include "ui_registrationexpirationreminder.h"

RegistrationExpirationReminder::RegistrationExpirationReminder(bool *is_show, QWidget *parent)
    : QDialog(parent)
    , m_is_show(is_show)
    , ui(new Ui::RegistrationExpirationReminder)
{
    ui->setupUi(this);

    init_ui();
    init_ui_text();
    init_connect();
}

RegistrationExpirationReminder::~RegistrationExpirationReminder()
{
    delete ui;
}

void RegistrationExpirationReminder::init_ui()
{
    this->loadCSS(this, "");

    this->setWindowTitle(tr("Registration Code Expiration Reminder"));
    ui->lab_message->setWordWrap(true);
}

void RegistrationExpirationReminder::init_ui_text()
{
    ui->lab_message->setText(tr("Your registration code will expired at %1 , please update the registration code as soon as possible, otherwise the expiration program will terminate automatically.").arg(Config::get_instance().get_end_time().toString("yyyy-MM-dd HH:mm:ss")));
    ui->checkBox->setText(tr("Don't show this reminder again"));
    // 确认
    ui->btn_verify->setText(tr("Verify"));
}

void RegistrationExpirationReminder::init_connect()
{

}

void RegistrationExpirationReminder::on_checkBox_stateChanged(int arg1)
{
    *m_is_show = !arg1;
}
