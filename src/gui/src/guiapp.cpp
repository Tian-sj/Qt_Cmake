#include "gui/guiapp.h"
#include "gui/mainwindow.h"
#include "gui/config.h"
#include "gui/registrationcodedialog.h"

#include <QApplication>
#include <QWidget>
#include <QMessageBox>

GuiApp::GuiApp(int &argc, char **argv)
    : app(std::make_unique<QApplication>(argc, argv))
    , main_window(nullptr)
{
}

GuiApp::~GuiApp() {
}

int GuiApp::run() {
    set_default_style();

    Config& config = Config::get_instance();

    config.set_language(config.get_language());
    config.set_theme(config.get_theme());

#if 0
    config.delete_registration_code();
#endif

    if (QDateTime::currentDateTime() <= config.get_app_runtime() || (!config.get_registration_code().isEmpty() && config.get_app_runtime().isNull())) {
        QMessageBox::critical(nullptr, "Error", "The application has expired, please contact the vendor for a new license key.");
        return 0;
    }

    config.set_app_runtime(QDateTime::currentDateTime());

    if (!config.validate_code(config.get_registration_code())) {
        RegistrationCodeDialog *reg_code_dialog = new RegistrationCodeDialog();
        if (reg_code_dialog->exec() != QDialog::Accepted)
            return 0;
    }

    main_window = std::make_unique<MainWindow>();
    main_window->show();
    return app->exec();
}

void GuiApp::set_default_style()
{
    app->setStyle("Fusion");

    QPalette palette;
    palette.setBrush(QPalette::Window, QColor("#F0F0F0"));
    app->setPalette(palette);}
