#include "views/main_window.hpp"

#include "cppproject/project_config.hpp"
#include "ui_main_window.h"
#include "views/registration_expiration_reminder.hpp"

#include <QWKWidgets/widgetwindowagent.h>
#include <QApplication>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QToolButton>
#include <QWidget>
#include <chrono>

namespace {

QString to_qstring(const std::string_view value) {
    return QString::fromUtf8(value.data(), static_cast<qsizetype>(value.size()));
}

} // namespace

MainWindow::MainWindow(cppproject::app_core::ApplicationService& application,
                       const cppproject::app_core::ApplicationStatus& startup_status,
                       QWidget* parent)
    : QMainWindow{parent}, application_{application}, ui_{std::make_unique<Ui::MainWindow>()} {
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    ui_->setupUi(this);
    install_window_agent();
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

void MainWindow::install_window_agent() {
    window_agent_ = new QWK::WidgetWindowAgent(this);
    window_agent_->setup(this);

    auto* title_bar = new QWidget(this);
    title_bar->setObjectName(QStringLiteral("windowTitleBar"));
    title_bar->setFixedHeight(38);

    auto* title_layout = new QHBoxLayout(title_bar);
#ifdef Q_OS_MAC
    title_layout->setContentsMargins(84, 0, 12, 0);
#else
    title_layout->setContentsMargins(12, 0, 0, 0);
#endif
    title_layout->setSpacing(0);

    auto* title_label = new QLabel(title_bar);
    title_label->setObjectName(QStringLiteral("windowTitleLabel"));
    title_label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    title_layout->addWidget(title_label, 1);
    connect(this, &QWidget::windowTitleChanged, title_label, &QLabel::setText);

#ifndef Q_OS_MAC
    auto make_button = [this, title_bar, title_layout](const QString& text,
                                                       const QString& object_name) {
        auto* button = new QToolButton(title_bar);
        button->setObjectName(object_name);
        button->setText(text);
        button->setFixedSize(46, 38);
        button->setAutoRaise(true);
        title_layout->addWidget(button);
        window_agent_->setHitTestVisible(button);
        return button;
    };

    auto* minimize_button =
        make_button(QStringLiteral("_"), QStringLiteral("windowMinimizeButton"));
    auto* maximize_button =
        make_button(QStringLiteral("[]"), QStringLiteral("windowMaximizeButton"));
    auto* close_button = make_button(QStringLiteral("X"), QStringLiteral("windowCloseButton"));

    window_agent_->setSystemButton(QWK::WindowAgentBase::Minimize, minimize_button);
    window_agent_->setSystemButton(QWK::WindowAgentBase::Maximize, maximize_button);
    window_agent_->setSystemButton(QWK::WindowAgentBase::Close, close_button);

    connect(minimize_button, &QToolButton::clicked, this, &QWidget::showMinimized);
    connect(maximize_button, &QToolButton::clicked, this, [this] {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });
    connect(close_button, &QToolButton::clicked, this, &QWidget::close);
#endif

    setMenuWidget(title_bar);
    window_agent_->setTitleBar(title_bar);
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        update_text();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::update_text() {
    ui_->retranslateUi(this);
    setWindowTitle(QStringLiteral("%1 %2").arg(to_qstring(cppproject::build::application_name),
                                               to_qstring(cppproject::build::version)));
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

void MainWindow::show_expiration_reminder(const cppproject::app_core::ApplicationStatus& status) {
    if (suppress_expiration_reminder_ || expiration_dialog_open_ || !status.expires_at) {
        return;
    }

    expiration_dialog_open_ = true;
    RegistrationExpirationReminder reminder{*status.expires_at, this};
    reminder.exec();
    suppress_expiration_reminder_ = reminder.suppress_future_reminders();
    expiration_dialog_open_ = false;
}

void MainWindow::close_for_issue(const cppproject::app_core::ApplicationIssue issue) {
    status_timer_.stop();
    const auto message =
        issue == cppproject::app_core::ApplicationIssue::clock_rollback
            ? tr("The system clock was moved backwards. The application will close.")
            : tr("The license has expired. The application will close.");
    QMessageBox::critical(this, tr("Application unavailable"), message);
    QApplication::quit();
}
