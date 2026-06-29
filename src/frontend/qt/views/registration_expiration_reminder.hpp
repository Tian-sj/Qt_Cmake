#pragma once

#include "qtcpp/application/ports.hpp"

#include <QDialog>
#include <memory>

namespace Ui {
class RegistrationExpirationReminder;
}

class RegistrationExpirationReminder final : public QDialog {
    Q_OBJECT

public:
    explicit RegistrationExpirationReminder(qtcpp::application::TimePoint expiration,
                                            QWidget* parent = nullptr);
    ~RegistrationExpirationReminder() override;

    [[nodiscard]] bool suppress_future_reminders() const;

protected:
    void changeEvent(QEvent* event) override;

private:
    void update_text();

    qtcpp::application::TimePoint expiration_;
    std::unique_ptr<Ui::RegistrationExpirationReminder> ui_;
};
