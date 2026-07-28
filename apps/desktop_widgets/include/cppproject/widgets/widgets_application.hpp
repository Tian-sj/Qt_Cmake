#pragma once

#include <memory>

class QApplication;
class MainWindow;

namespace cppproject::app_core {
class ApplicationService;
}

namespace cppproject::widgets {

class UiSettings;

class WidgetsApplication final {
public:
    WidgetsApplication(int& argc, char** argv, app_core::ApplicationService& application);
    ~WidgetsApplication();

    WidgetsApplication(const WidgetsApplication&) = delete;
    WidgetsApplication& operator=(const WidgetsApplication&) = delete;

    int run();

private:
    void apply_default_style();

    app_core::ApplicationService& application_;
    std::unique_ptr<QApplication> qt_application_;
    std::unique_ptr<UiSettings> ui_settings_;
    std::unique_ptr<MainWindow> main_window_;
};

} // namespace cppproject::widgets
