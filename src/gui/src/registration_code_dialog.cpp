#include "gui/registration_code_dialog.hpp"
#include "gui/config.hpp"

#include <QClipboard>
#include <QFile>
#include <QApplication>

#include "ui_registration_code_dialog.h"

RegistrationCodeDialog::RegistrationCodeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegistrationCodeDialog)
{
    ui->setupUi(this);

    initUi();
    initUiText();
    initConnect();
}

RegistrationCodeDialog::~RegistrationCodeDialog()
{
    delete ui;
}

void RegistrationCodeDialog::initUi()
{
    this->loadCSS(this, "");
    ui->lab_uuid->setText("UUID");
    ui->uuid->setText(Config::getInstance().getUniqueSystemIdentifier());
}

void RegistrationCodeDialog::initUiText()
{
    ui->btn_copy->setText(tr("Copy"));
    ui->btn_activate->setText(tr("Activate"));
    ui->text->setPlainText(Config::getInstance().getRegistrationCode());
}

void RegistrationCodeDialog::initConnect()
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
        case RegistrationCode::ErrorType::REGISTRATION_CODE_INVALID_FORMAT:
            ui->lab_error->setText(tr("Invalid registration code format"));
            break;
        case RegistrationCode::ErrorType::REGISTRATION_CODE_EXPIRED:
            ui->lab_error->setText(tr("Registration code expired"));
            break;
        case RegistrationCode::ErrorType::REGISTRATION_CODE_INVALID:
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
