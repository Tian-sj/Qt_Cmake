#include "views/registration_expiration_reminder.hpp"

#include "ui_registration_expiration_reminder.h"

#include <QDateTime>
#include <QEvent>
#include <chrono>

RegistrationExpirationReminder::RegistrationExpirationReminder(
    const qtcpp::application::TimePoint expiration, QWidget* parent)
    : QDialog{parent}, expiration_{expiration},
      ui_{std::make_unique<Ui::RegistrationExpirationReminder>()} {
    ui_->setupUi(this);
    ui_->lab_message->setWordWrap(true);
    update_text();
}

RegistrationExpirationReminder::~RegistrationExpirationReminder() = default;

bool RegistrationExpirationReminder::suppress_future_reminders() const {
    return ui_->check_box->isChecked();
}

void RegistrationExpirationReminder::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        update_text();
    }
    QDialog::changeEvent(event);
}

void RegistrationExpirationReminder::update_text() {
    ui_->retranslateUi(this);
    const auto seconds =
        std::chrono::duration_cast<std::chrono::seconds>(expiration_.time_since_epoch()).count();
    const auto expiration_text =
        QDateTime::fromSecsSinceEpoch(seconds).toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));

    setWindowTitle(tr("Registration code expiration reminder"));
    ui_->lab_message->setText(
        tr("Your registration code expires at %1. Update it before that time.")
            .arg(expiration_text));
    ui_->check_box->setText(tr("Don't show this reminder again"));
    ui_->btn_verify->setText(tr("OK"));
}
