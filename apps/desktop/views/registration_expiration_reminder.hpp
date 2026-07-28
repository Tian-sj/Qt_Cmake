#pragma once

#include "cppproject/app_core/ports.hpp"

#include <QDialog>
#include <memory>

namespace Ui {
class RegistrationExpirationReminder;
}

class RegistrationExpirationReminder final : public QDialog {
    Q_OBJECT

public:
    explicit RegistrationExpirationReminder(cppproject::app_core::TimePoint expiration,
                                            QWidget* parent = nullptr);
    ~RegistrationExpirationReminder() override;

    [[nodiscard]] bool suppress_future_reminders() const;

protected:
    void changeEvent(QEvent* event) override;

private:
    void update_text();

    cppproject::app_core::TimePoint expiration_;
    std::unique_ptr<Ui::RegistrationExpirationReminder> ui_;
};
