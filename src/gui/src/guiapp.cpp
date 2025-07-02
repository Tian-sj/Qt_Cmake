#include "gui/guiapp.hpp"
#include "gui/mainwindow.hpp"
#include "gui/config.hpp"
#include "gui/registrationcodedialog.hpp"

#include <QApplication>
#include <QWidget>
#include <QMessageBox>
#include <QFontDatabase>
#include <QChar>

GuiApp::GuiApp(int &argc, char **argv)
    : app(std::make_unique<QApplication>(argc, argv))
    , main_window(nullptr)
{
}

GuiApp::~GuiApp() {
}

int GuiApp::run() {
    set_default_style();

    Config& config = Config::getInstance();

    config.setLanguage(config.getLanguage());
    config.setTheme(config.getTheme());

    QFont app_font = config.getFont(Config::Font::Inter, 14, QFont::Normal);
    app_font.setStyleStrategy(static_cast<QFont::StyleStrategy>(QFont::PreferDefault | QFont::ContextFontMerging));
    app->setFont(app_font);

    QFontDatabase::setApplicationFallbackFontFamilies(QChar::Script_Han, config.getFontFamilies(Config::Font::Noto_Sans_SC));

#if 0
    config.deleteRegistrationCode();
#endif

    if (QDateTime::currentDateTime() <= config.getAppRuntime() || (!config.getRegistrationCode().isEmpty() && config.getAppRuntime().isNull())) {
        QMessageBox::critical(nullptr, "Error", "The application has expired, please contact the vendor for a new license key.");
        return 0;
    }

    config.setAppRuntime(QDateTime::currentDateTime());

    if (!config.validateCode(config.getRegistrationCode())) {
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
