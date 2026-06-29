#pragma once

#include <memory>

class QApplication;
class MainWindow;

namespace qtcpp::application {
class ApplicationService;
}

namespace qtcpp::gui {

class UiSettings;

class GuiApplication final {
public:
    GuiApplication(int& argc, char** argv, application::ApplicationService& application);
    ~GuiApplication();

    GuiApplication(const GuiApplication&) = delete;
    GuiApplication& operator=(const GuiApplication&) = delete;

    int run();

private:
    void apply_default_style();

    application::ApplicationService& application_;
    std::unique_ptr<QApplication> qt_application_;
    std::unique_ptr<UiSettings> ui_settings_;
    std::unique_ptr<MainWindow> main_window_;
};

} // namespace qtcpp::gui
