#include "views/registration_code_dialog.hpp"

#include "ui_registration_code_dialog.h"

#include <QApplication>
#include <QClipboard>
#include <QEvent>

RegistrationCodeDialog::RegistrationCodeDialog(qtcpp::application::ApplicationService& application,
                                               QWidget* parent)
    : QDialog{parent}, application_{application},
      ui_{std::make_unique<Ui::RegistrationCodeDialog>()} {
    ui_->setupUi(this);
    ui_->uuid->setText(QString::fromUtf8(application_.machine_identifier()));
    ui_->text->setPlainText(QString::fromUtf8(application_.saved_license_code()));

    connect(ui_->btn_copy, &QPushButton::clicked, this,
            &RegistrationCodeDialog::copy_machine_identifier);
    connect(ui_->btn_activate, &QPushButton::clicked, this, &RegistrationCodeDialog::activate);
    update_text();
}

RegistrationCodeDialog::~RegistrationCodeDialog() = default;

void RegistrationCodeDialog::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        update_text();
    }
    QDialog::changeEvent(event);
}

void RegistrationCodeDialog::update_text() {
    ui_->retranslateUi(this);
    setWindowTitle(tr("License activation"));
    ui_->lab_uuid->setText(tr("Machine identifier"));
    ui_->btn_copy->setText(tr("Copy"));
    ui_->btn_activate->setText(tr("Activate"));
}

void RegistrationCodeDialog::copy_machine_identifier() const {
    QApplication::clipboard()->setText(ui_->uuid->text());
}

void RegistrationCodeDialog::activate() {
    const auto code = ui_->text->toPlainText().toUtf8();
    activation_result_ = application_.activate(
        std::string_view{code.constData(), static_cast<std::size_t>(code.size())});
    if (activation_result_.accepted()) {
        accept();
        return;
    }
    show_error(activation_result_.status);
}

void RegistrationCodeDialog::show_error(const qtcpp::application::LicenseStatus status) {
    switch (status) {
    case qtcpp::application::LicenseStatus::invalid_format:
        ui_->lab_error->setText(tr("Invalid registration code format"));
        break;
    case qtcpp::application::LicenseStatus::expired:
        ui_->lab_error->setText(tr("Registration code expired"));
        break;
    case qtcpp::application::LicenseStatus::invalid:
        ui_->lab_error->setText(tr("Invalid registration code"));
        break;
    case qtcpp::application::LicenseStatus::valid:
    case qtcpp::application::LicenseStatus::about_to_expire:
        ui_->lab_error->clear();
        break;
    }
}
