#pragma once

#include "qtcpp/application/application_service.hpp"

#include <QMainWindow>
#include <QTimer>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(qtcpp::application::ApplicationService& application,
               const qtcpp::application::ApplicationStatus& startup_status,
               QWidget* parent = nullptr);
    ~MainWindow() override;

    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

protected:
    void changeEvent(QEvent* event) override;

private:
    void update_text();
    void check_application_status();
    void show_expiration_reminder(const qtcpp::application::ApplicationStatus& status);
    void close_for_issue(qtcpp::application::ApplicationIssue issue);

    qtcpp::application::ApplicationService& application_;
    std::unique_ptr<Ui::MainWindow> ui_;
    QTimer status_timer_;
    bool suppress_expiration_reminder_{false};
    bool expiration_dialog_open_{false};
};
