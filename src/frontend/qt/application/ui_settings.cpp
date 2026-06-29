#include "application/ui_settings.hpp"

#include "qtcpp/project_config.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QDirIterator>
#include <QFile>
#include <QFontDatabase>
#include <QLocale>
#include <QStyle>
#include <QTranslator>
#include <string>

namespace qtcpp::gui {
namespace {

QString to_qstring(const std::string_view value) {
    return QString::fromUtf8(value.data(), static_cast<qsizetype>(value.size()));
}

} // namespace

UiSettings::UiSettings(QApplication& application)
    : application_{application},
      settings_{QSettings::IniFormat, QSettings::UserScope, to_qstring(build::organization),
                to_qstring(build::application_name)} {
    load_bundled_fonts();
}

UiSettings::~UiSettings() = default;

void UiSettings::apply_saved_preferences() {
    set_language(language());

    QFile stylesheet{QStringLiteral(":/GUI/css/Light/gui-themes.css")};
    if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) {
        application_.setStyleSheet(QString::fromUtf8(stylesheet.readAll()));
    }
}

void UiSettings::set_language(const Language language) {
    if (translator_) {
        application_.removeTranslator(translator_.get());
        translator_.reset();
    }

    settings_.setValue(QStringLiteral("ui/language"), language == Language::simplified_chinese
                                                          ? QStringLiteral("zh_CN")
                                                          : QStringLiteral("en"));

    if (language == Language::simplified_chinese) {
        auto translator = std::make_unique<QTranslator>();
        if (translator->load(QStringLiteral(":/i18n/gui_zh_CN.qm"))) {
            application_.installTranslator(translator.get());
            translator_ = std::move(translator);
        }
    }
}

UiSettings::Language UiSettings::language() const {
    const auto fallback = system_language() == Language::simplified_chinese
                              ? QStringLiteral("zh_CN")
                              : QStringLiteral("en");
    const auto saved = settings_.value(QStringLiteral("ui/language"), fallback).toString();
    return saved == QStringLiteral("zh_CN") ? Language::simplified_chinese : Language::english;
}

QFont UiSettings::application_font(const int point_size) const {
    QFont font = application_.font();
    if (!font_family_.isEmpty()) {
        font.setFamily(font_family_);
    }
    font.setPointSize(point_size);
    return font;
}

void UiSettings::load_bundled_fonts() {
#if defined(Q_OS_MACOS)
    const auto fonts_path =
        QCoreApplication::applicationDirPath() + QStringLiteral("/../Resources/fonts");
#else
    const auto fonts_path = QCoreApplication::applicationDirPath() + QStringLiteral("/fonts");
#endif
    QDirIterator files{fonts_path,
                       {QStringLiteral("*.ttf"), QStringLiteral("*.otf")},
                       QDir::Files,
                       QDirIterator::Subdirectories};

    while (files.hasNext()) {
        const auto identifier = QFontDatabase::addApplicationFont(files.next());
        if (identifier < 0) {
            continue;
        }
        const auto families = QFontDatabase::applicationFontFamilies(identifier);
        if (font_family_.isEmpty() && !families.isEmpty()) {
            font_family_ = families.front();
        }
    }
}

UiSettings::Language UiSettings::system_language() {
    return QLocale::system().language() == QLocale::Chinese ? Language::simplified_chinese
                                                            : Language::english;
}

} // namespace qtcpp::gui
