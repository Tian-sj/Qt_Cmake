#pragma once

#include "cppproject/app_core/application_service.hpp"

#include <QDialog>
#include <memory>

namespace Ui {
class RegistrationCodeDialog;
}

class RegistrationCodeDialog final : public QDialog {
    Q_OBJECT

public:
    explicit RegistrationCodeDialog(cppproject::app_core::ApplicationService& application,
                                    QWidget* parent = nullptr);
    ~RegistrationCodeDialog() override;

protected:
    void changeEvent(QEvent* event) override;

private:
    void update_text();
    void copy_machine_identifier() const;
    void activate();
    void show_error(cppproject::app_core::LicenseStatus status);

    cppproject::app_core::ApplicationService& application_;
    std::unique_ptr<Ui::RegistrationCodeDialog> ui_;
    cppproject::app_core::LicenseResult activation_result_;
};
