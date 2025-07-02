#include "gui/registrationcodedialog.hpp"
#include "gui/config.hpp"

#include <QClipboard>
#include <QFile>
#include <QApplication>

#include "ui_registrationcodedialog.h"

RegistrationCodeDialog::RegistrationCodeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegistrationCodeDialog)
{
    ui->setupUi(this);

    init_ui();
    init_ui_text();
    init_connect();
}

RegistrationCodeDialog::~RegistrationCodeDialog()
{
    delete ui;
}

void RegistrationCodeDialog::init_ui()
{
    this->loadCSS(this, "");
    ui->lab_uuid->setText("UUID");
    ui->uuid->setText(Config::getInstance().getUniqueSystemIdentifier());
}

void RegistrationCodeDialog::init_ui_text()
{
    ui->btn_copy->setText(tr("Copy"));
    ui->btn_activate->setText(tr("Activate"));
    ui->text->setPlainText(Config::getInstance().getRegistrationCode());
}

void RegistrationCodeDialog::init_connect()
{

}

void RegistrationCodeDialog::on_btn_copy_clicked()
{
    QClipboard* clip = QApplication::clipboard();
    clip->setText(ui->uuid->text());
}


void RegistrationCodeDialog::on_btn_activate_clicked()
{
    QString code = ui->text->toPlainText();
    bool valid =  Config::getInstance().validateCode(code);
    if (valid) {
        Config::getInstance().setRegistrationCode(code);
        return accept();
    } else {
        switch (Config::getInstance().getRegistrationCodeErrorType())
        {
        case RegistrationCode::ERROT_TYPE::RegistrationCodeInvalidFormat:
            ui->lab_error->setText(tr("Invalid registration code format"));
            break;
        case RegistrationCode::ERROT_TYPE::RegistrationCodeExpired:
            ui->lab_error->setText(tr("Registration code expired"));
            break;
        case RegistrationCode::ERROT_TYPE::RegistrationCodeInvalid:
            ui->lab_error->setText(tr("Invalid registration code"));
            break;
        default:
            break;
        }
    }
}

void RegistrationCodeDialog::closeEvent(QCloseEvent *event)
{
    return reject();
}
