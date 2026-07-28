#pragma once

#include "cppproject/app_core/application_service.hpp"

#include <QMainWindow>
#include <QTimer>
#include <memory>

namespace Ui {
class MainWindow;
}

namespace QWK {
class WidgetWindowAgent;
}

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(cppproject::app_core::ApplicationService& application,
               const cppproject::app_core::ApplicationStatus& startup_status,
               QWidget* parent = nullptr);
    ~MainWindow() override;

    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

protected:
    void changeEvent(QEvent* event) override;

private:
    void install_window_agent();
    void update_text();
    void check_application_status();
    void show_expiration_reminder(const cppproject::app_core::ApplicationStatus& status);
    void close_for_issue(cppproject::app_core::ApplicationIssue issue);

    cppproject::app_core::ApplicationService& application_;
    std::unique_ptr<Ui::MainWindow> ui_;
    QWK::WidgetWindowAgent* window_agent_{nullptr};
    QTimer status_timer_;
    bool suppress_expiration_reminder_{false};
    bool expiration_dialog_open_{false};
};
