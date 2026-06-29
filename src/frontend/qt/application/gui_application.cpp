#include "qtcpp/gui/gui_application.hpp"

#include "application/ui_settings.hpp"
#include "qtcpp/application/application_service.hpp"
#include "views/main_window.hpp"
#include "views/registration_code_dialog.hpp"

#include <QApplication>
#include <QMessageBox>
#include <QPalette>
#include <cstdlib>
#include <memory>

namespace qtcpp::gui {

GuiApplication::GuiApplication(int& argc, char** argv, application::ApplicationService& application)
    : application_{application}, qt_application_{std::make_unique<QApplication>(argc, argv)} {}

GuiApplication::~GuiApplication() = default;

int GuiApplication::run() {
    apply_default_style();
    ui_settings_ = std::make_unique<UiSettings>(*qt_application_);
    ui_settings_->apply_saved_preferences();

#if defined(_WIN32)
    constexpr int font_size = 10;
#else
    constexpr int font_size = 12;
#endif
    qt_application_->setFont(ui_settings_->application_font(font_size));

    auto status = application_.start();
    if (status.issue == application::ApplicationIssue::clock_rollback) {
        QMessageBox::critical(
            nullptr, QApplication::translate("GuiApplication", "Startup blocked"),
            QApplication::translate(
                "GuiApplication", "The system clock is earlier than the last recorded run time."));
        return EXIT_FAILURE;
    }

    if (status.issue == application::ApplicationIssue::activation_required ||
        status.issue == application::ApplicationIssue::license_expired) {
        RegistrationCodeDialog dialog{application_};
        if (dialog.exec() != QDialog::Accepted) {
            return EXIT_SUCCESS;
        }

        status = application_.poll();
    }

    main_window_ = std::make_unique<MainWindow>(application_, status);
    main_window_->show();
    return qt_application_->exec();
}

void GuiApplication::apply_default_style() {
    qt_application_->setStyle(QStringLiteral("Fusion"));
    auto palette = qt_application_->palette();
    palette.setColor(QPalette::Window, QColor::fromRgb(0xF0, 0xF0, 0xF0));
    qt_application_->setPalette(palette);
}

} // namespace qtcpp::gui
