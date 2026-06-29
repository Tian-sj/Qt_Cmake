#pragma once

#include "qtcpp/application/application_service.hpp"

#include <QDialog>
#include <memory>

namespace Ui {
class RegistrationCodeDialog;
}

class RegistrationCodeDialog final : public QDialog {
    Q_OBJECT

public:
    explicit RegistrationCodeDialog(qtcpp::application::ApplicationService& application,
                                    QWidget* parent = nullptr);
    ~RegistrationCodeDialog() override;

protected:
    void changeEvent(QEvent* event) override;

private:
    void update_text();
    void copy_machine_identifier() const;
    void activate();
    void show_error(qtcpp::application::LicenseStatus status);

    qtcpp::application::ApplicationService& application_;
    std::unique_ptr<Ui::RegistrationCodeDialog> ui_;
    qtcpp::application::LicenseResult activation_result_;
};
