#include "views/main_window.hpp"

#include "qtcpp/project_config.hpp"
#include "ui_main_window.h"
#include "views/registration_expiration_reminder.hpp"

#include <QApplication>
#include <QEvent>
#include <QMessageBox>
#include <QTimer>
#include <chrono>

namespace {

QString to_qstring(const std::string_view value) {
    return QString::fromUtf8(value.data(), static_cast<qsizetype>(value.size()));
}

} // namespace

MainWindow::MainWindow(qtcpp::application::ApplicationService& application,
                       const qtcpp::application::ApplicationStatus& startup_status, QWidget* parent)
    : QMainWindow{parent}, application_{application}, ui_{std::make_unique<Ui::MainWindow>()} {
    ui_->setupUi(this);
    update_text();

    status_timer_.setInterval(std::chrono::minutes{1});
    connect(&status_timer_, &QTimer::timeout, this, &MainWindow::check_application_status);
    if (application_.licensing_enabled()) {
        status_timer_.start();
    }

    if (startup_status.expiration_warning) {
        QTimer::singleShot(0, this,
                           [this, startup_status] { show_expiration_reminder(startup_status); });
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        update_text();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::update_text() {
    ui_->retranslateUi(this);
    setWindowTitle(QStringLiteral("%1 %2").arg(to_qstring(qtcpp::build::application_name),
                                               to_qstring(qtcpp::build::version)));
}

void MainWindow::check_application_status() {
    const auto status = application_.poll();
    if (!status.can_continue()) {
        close_for_issue(status.issue);
        return;
    }
    if (status.expiration_warning) {
        show_expiration_reminder(status);
    }
}

void MainWindow::show_expiration_reminder(const qtcpp::application::ApplicationStatus& status) {
    if (suppress_expiration_reminder_ || expiration_dialog_open_ || !status.expires_at) {
        return;
    }

    expiration_dialog_open_ = true;
    RegistrationExpirationReminder reminder{*status.expires_at, this};
    reminder.exec();
    suppress_expiration_reminder_ = reminder.suppress_future_reminders();
    expiration_dialog_open_ = false;
}

void MainWindow::close_for_issue(const qtcpp::application::ApplicationIssue issue) {
    status_timer_.stop();
    const auto message =
        issue == qtcpp::application::ApplicationIssue::clock_rollback
            ? tr("The system clock was moved backwards. The application will close.")
            : tr("The license has expired. The application will close.");
    QMessageBox::critical(this, tr("Application unavailable"), message);
    QApplication::quit();
}
