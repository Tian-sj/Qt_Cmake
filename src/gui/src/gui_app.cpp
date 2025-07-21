#include "gui/gui_app.hpp"
#include "gui/main_window.hpp"
#include "gui/config.hpp"
#include "gui/registration_code_dialog.hpp"

#include <QApplication>
#include <QWidget>
#include <QMessageBox>
#include <QFontDatabase>


#if(_WIN32)
static constexpr int FONT_SIZE = 10;
#else
static constexpr int FONT_SIZE = 12;
#endif

GuiApp::GuiApp(int &argc, char **argv)
    : app_(std::make_unique<QApplication>(argc, argv))
    , main_window_(nullptr)
{
}

GuiApp::~GuiApp() {
}

int GuiApp::run() {
    setDefaultStyle();

    Config& config = Config::getInstance();

    // config.setLanguage(Config::Language::Chinese);
    config.setLanguage(config.getLanguage());
    config.setTheme(config.getTheme());

    QFont app_font = config.getFont(Config::Font::HarmonyOS_Sans, FONT_SIZE, QFont::Normal);
    // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
    QFont::StyleStrategy strategy = QFont::StyleStrategy(QFont::PreferDefault | QFont::ContextFontMerging);
    app_->setFont(app_font);

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

    main_window_ = std::make_unique<MainWindow>();
    main_window_->show();
    return app_->exec();
}

void GuiApp::setDefaultStyle()
{
    app_->setStyle("Fusion");

    QPalette palette;
    palette.setBrush(QPalette::Window, QColor::fromRgb(0xF0F0F0));
    app_->setPalette(palette);}
