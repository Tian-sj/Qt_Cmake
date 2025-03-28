#include "gui/config.h"

#include <QApplication>
#include <QFontDatabase>
#include <QSettings>
#include <QTranslator>
#include <QResource>
#include <QDir>
#include <QFile>
#include <QWidget>

Config &Config::get_instance()
{
    static Config instance;
    return instance;
}

Config::Config() 
    : m_settings(new QSettings(QSettings::NativeFormat, QSettings::UserScope, _DEVELOPER, _NAME))
    , m_reg_code(new RegistrationCode(QString("%1_%2").arg(_DEVELOPER, _NAME).toLocal8Bit()))
{
    m_settings->setValue("version", _VERSION);

    get_translation_file_names();
    add_font();
}

Config::~Config()
{
    if (m_settings) {
        delete m_settings;
        m_settings = nullptr;
    }

    foreach (auto translator, translators) {
        delete translator;
        translator = nullptr;
    }

    translators.clear();
}

void Config::set_language(const Language &language)
{
    switch (language) {
    case Language::English:
        set_font(Font::Noto_Serif);
        break;
    case Language::Chinese:
        // set_font(Font::Noto_Sans_SC);
        set_font(Font::Noto_Serif);
        break;
    default:
        break;
    }

    auto name_view = magic_enum::enum_name(language);
    m_settings->setValue("Language", QString::fromUtf8(name_view.data(), static_cast<int>(name_view.size())));

    QStringList list = map_language_path[language];
    size_t i { 0 };
    foreach (const auto &file_path, list) {
        qDebug() << file_path << ":" << translators[i]->load(file_path);
        qApp->installTranslator(translators[i++]);
    }
}

Config::Language Config::get_language() const
{
    auto name = magic_enum::enum_cast<Language>(m_settings->value("Language").toString().toStdString());
    if (name.has_value())
        return name.value();
    else
        return Language::English;
}

void Config::set_theme(const Theme &theme)
{
    auto name_view = magic_enum::enum_name(theme);
    QString path = QString::fromUtf8(name_view.data(), name_view.size());
    m_settings->setValue("Theme", path);
    theme_path = ":/GUI/css/" + path + "/";
}

QString Config::get_theme_path() const
{
    return theme_path;
}

Config::Theme Config::get_theme() const
{
    auto name = magic_enum::enum_cast<Theme>(m_settings->value("Theme").toString().toStdString());
    if (name.has_value())
        return name.value();
    else
        return Theme::Light;
}

void Config::set_current_user_id(const int &user_id)
{
    this->m_user_id = user_id;
}

int Config::get_current_user_id() const
{
    return this->m_user_id;
}

void Config::set_app_runtime(QDateTime runtime)
{
    m_settings->setValue("App_runtime", runtime);
}

QDateTime Config::get_app_runtime() const
{
    return m_settings->value("App_runtime").toDateTime();
}

void Config::set_font(const Font &font)
{
    QFont f(map_font_name[font]);
    qApp->setFont(f);
}

void Config::display_font_families()
{
    QStringList fontFamilies = QFontDatabase::families();
    qDebug() << "Available Fonts:";
    for (const auto& family : fontFamilies) {
        qDebug() << family;
    }
}

void Config::registerResource(QString file)
{
    qDebug() <<"registerResource:" << QResource::registerResource(file);
}

bool Config::validate_code(const QString &code)
{
    m_registration_code_error_type = m_reg_code->validateCode(code);
    switch (m_registration_code_error_type)
    {
    case RegistrationCode::ERROT_TYPE::RegistrationCodeValid:
    case RegistrationCode::ERROT_TYPE::RegistrationCodeAboutToExpire:
        return true;
    case RegistrationCode::ERROT_TYPE::RegistrationCodeInvalid:
    case RegistrationCode::ERROT_TYPE::RegistrationCodeInvalidFormat:
    case RegistrationCode::ERROT_TYPE::RegistrationCodeExpired:
        return false;
    default:
        return false;
    }
}

void Config::set_registration_code(const QString& code) const
{
    m_settings->setValue("RegistrationCode", code.toLocal8Bit());
}

void Config::delete_registration_code() const
{
    m_settings->remove("RegistrationCode");
}

QString Config::get_registration_code() const
{
    return QString::fromLocal8Bit(m_settings->value("RegistrationCode").toByteArray());
}

QString Config::get_unique_system_identifier() const
{
    return m_reg_code->getUniqueSystemIdentifier();
}

bool Config::check_expiration_reminder()
{
    if (m_registration_code_error_type == RegistrationCode::ERROT_TYPE::RegistrationCodeAboutToExpire)
        return true;
    
    if (is_within_x_days(3, QDateTime::currentDateTime(), m_reg_code->getEndTime())) {
        m_registration_code_error_type = RegistrationCode::ERROT_TYPE::RegistrationCodeAboutToExpire;
        return true;
    }

    return false;
}

QDateTime Config::get_end_time() const
{
    return m_reg_code->getEndTime();
}

RegistrationCode::ERROT_TYPE Config::get_registration_code_error_type() const
{
    return m_registration_code_error_type;
}

bool Config::is_within_x_days(const int day, const QDateTime& date_time_1, const QDateTime& date_time_2)
{
    int days = qAbs(date_time_1.daysTo(date_time_2));
    return days < day;
}

void Config::get_translation_file_names()
{
    QString resourcePath = ":/GUI/languages/";
    QDir dir(resourcePath);

    QStringList filters;
    QFileInfoList fileList;
    filters << "*_en.qm";
    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files);
    fileList = dir.entryInfoList();
    foreach (const QFileInfo &fileInfo, fileList) {
        map_language_path[Language::English] << fileInfo.absoluteFilePath();
        translators.emplace_back(new QTranslator);
    }

    filters.clear();
    filters << "*_cn.qm";
    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files);
    fileList = dir.entryInfoList();
    foreach (const QFileInfo &fileInfo, fileList) {
        map_language_path[Language::Chinese] << fileInfo.absoluteFilePath();
    }
}

void Config::add_font()
{
    int id;
    id = QFontDatabase::addApplicationFont(":/GUI/fonts/NotoSansSC-VariableFont_wght.ttf");
    map_font_name[Font::Noto_Sans_SC] = QFontDatabase::applicationFontFamilies(id).at(0);
    id = QFontDatabase::addApplicationFont(":/GUI/fonts/NotoSerif-Italic-VariableFont_wdth,wght.ttf");
    id = QFontDatabase::addApplicationFont(":/GUI/fonts/NotoSerif-VariableFont_wdth,wght.ttf");
    map_font_name[Font::Noto_Serif] = QFontDatabase::applicationFontFamilies(id).at(0);
}
