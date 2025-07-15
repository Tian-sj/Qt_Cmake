#include "gui/config.hpp"

#include <magic_enum/magic_enum.hpp>
#include <QApplication>
#include <QFontDatabase>
#include <QSettings>
#include <QTranslator>
#include <QResource>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QWidget>

Config &Config::getInstance() {
    static Config instance;
    return instance;
}

Config::Config()
    : settings_(new QSettings(QSettings::NativeFormat, QSettings::UserScope, _DEVELOPER, _NAME))
    , reg_code_(new RegistrationCode(QString("%1_%2").arg(_DEVELOPER, _NAME).toLocal8Bit())) {
    settings_->setValue("version", _VERSION);

    getTranslationFileNames();
    loadFonts();
}

Config::~Config() {
    if (settings_) {
        delete settings_;
        settings_ = nullptr;
    }

    foreach (auto translator, translators_) {
        delete translator;
        translator = nullptr;
    }

    translators_.clear();
}

void Config::setLanguage(const Language &language) {
    auto name_view = magic_enum::enum_name(language);
    settings_->setValue("Language", QString::fromUtf8(name_view.data(), static_cast<int>(name_view.size())));

    QStringList list = map_language_path_[language];
    size_t i { 0 };
    foreach (const auto &file_path, list) {
        qDebug() << file_path << ":" << translators_[i]->load(file_path);
        qApp->installTranslator(translators_[i++]);
    }
}

Config::Language Config::getLanguage() const {
    auto name = magic_enum::enum_cast<Language>(settings_->value("Language").toString().toStdString());
    if (name.has_value())
        return name.value();
    else
        return Language::English;
}

void Config::setTheme(const Theme &theme) {
    auto name_view = magic_enum::enum_name(theme);
    QString path = QString::fromUtf8(name_view.data(), name_view.size());
    settings_->setValue("Theme", path);
    theme_path_ = ":/GUI/css/" + path + "/";
}

QString Config::getThemePath() const {
    return theme_path_;
}

Config::Theme Config::getTheme() const {
    auto name = magic_enum::enum_cast<Theme>(settings_->value("Theme").toString().toStdString());
    if (name.has_value())
        return name.value();
    else
        return Theme::Light;
}

QFont Config::getFont(Font primary, int point_size, QFont::Weight weight) {
    if (!font_families_.contains(primary) || font_families_[primary].isEmpty()) {
        qWarning() << "[FontManager] Font not found for enum:" << static_cast<int>(primary);
        return QFont();
    }

    QString family = font_families_[primary].first();
    QFont font(family);
    font.setPointSize(point_size);
    font.setWeight(weight);
    return font;
}

QStringList Config::getFontFamilies(Font font) {
    return font_families_[font];
}

void Config::setCurrentUserId(const int &user_id) {
    this->user_id_ = user_id;
}

int Config::getCurrentUserId() const {
    return this->user_id_;
}

void Config::setAppRuntime(QDateTime runtime) {
    settings_->setValue("App_runtime", runtime);
}

QDateTime Config::getAppRuntime() const {
    return settings_->value("App_runtime").toDateTime();
}

void Config::registerResource(QString file) {
    qDebug() <<"registerResource:" << QResource::registerResource(file);
}

bool Config::validateCode(const QString &code) {
    registration_code_error_type_ = reg_code_->validateCode(code);
    switch (registration_code_error_type_) {
    case RegistrationCode::ErrorType::REGISTRATION_CODE_VALID:
    case RegistrationCode::ErrorType::REGISTRATION_CODE_ABOUT_TO_EXPIRE:
        return true;
    case RegistrationCode::ErrorType::REGISTRATION_CODE_INVALID:
    case RegistrationCode::ErrorType::REGISTRATION_CODE_INVALID_FORMAT:
    case RegistrationCode::ErrorType::REGISTRATION_CODE_EXPIRED:
        return false;
    default:
        return false;
    }
}

void Config::setRegistrationCode(const QString& code) const {
    settings_->setValue("RegistrationCode", code.toLocal8Bit());
}

void Config::deleteRegistrationCode() const {
    settings_->remove("RegistrationCode");
}

QString Config::getRegistrationCode() const {
    return QString::fromLocal8Bit(settings_->value("RegistrationCode").toByteArray());
}

QString Config::getUniqueSystemIdentifier() const {
    return reg_code_->getUniqueSystemIdentifier();
}

bool Config::checkExpirationReminder() {
    if (registration_code_error_type_ == RegistrationCode::ErrorType::REGISTRATION_CODE_ABOUT_TO_EXPIRE)
        return true;
    
    if (isWithinXDays(3, QDateTime::currentDateTime(), reg_code_->getEndTime())) {
        registration_code_error_type_ = RegistrationCode::ErrorType::REGISTRATION_CODE_ABOUT_TO_EXPIRE;
        return true;
    }

    return false;
}

QDateTime Config::getEndTime() const {
    return reg_code_->getEndTime();
}

RegistrationCode::ErrorType Config::getRegistrationCodeErrorType() const {
    return registration_code_error_type_;
}

bool Config::isWithinXDays(const int day, const QDateTime& date_time_1, const QDateTime& date_time_2) {
    int days = qAbs(date_time_1.daysTo(date_time_2));
    return days < day;
}

void Config::getTranslationFileNames() {
    QString resource_path = ":/GUI/languages/";
    QDir dir(resource_path);

    QStringList filters;
    QFileInfoList file_list;
    filters << "*_en.qm";
    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files);
    file_list = dir.entryInfoList();
    foreach (const QFileInfo &file_info, file_list) {
        map_language_path_[Language::English] << file_info.absoluteFilePath();
        translators_.emplace_back(new QTranslator);
    }

    filters.clear();
    filters << "*_cn.qm";
    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files);
    file_list = dir.entryInfoList();
    foreach (const QFileInfo &file_info, file_list) {
        map_language_path_[Language::Chinese] << file_info.absoluteFilePath();
    }
}

QString Config::fontDir(Font font) {
    switch (font) {
    case Font::HarmonyOS_Sans: return QCoreApplication::applicationDirPath() + "/fonts/HarmonyOS_Sans";
    case Font::HarmonyOS_Sans_SC : return QCoreApplication::applicationDirPath() + "/fonts/HarmonyOS_Sans_SC";
    default: return "";
    }
}

void Config::loadFonts() {
    loadFontDir(Font::HarmonyOS_Sans, fontDir(Font::HarmonyOS_Sans));
    loadFontDir(Font::HarmonyOS_Sans_SC, fontDir(Font::HarmonyOS_Sans_SC));
}

void Config::loadFontDir(Font font, const QString &dir_path) {
    QStringList families;

    // 遍历资源目录下的所有 .ttf / .otf 字体文件
    QDirIterator it(dir_path, QStringList() << "*.ttf" << "*.otf", QDir::Files);
    while (it.hasNext()) {
        QString file_path = it.next();
        int id = QFontDatabase::addApplicationFont(file_path);
        if (id >= 0) {
            QStringList loaded = QFontDatabase::applicationFontFamilies(id);
            if (!loaded.isEmpty()) {
                families << loaded.first();
                qDebug() << "[FontManager] Loaded:" << file_path << "->" << loaded.first();
            }
        } else {
            qWarning() << "[FontManager] Failed to load:" << file_path;
        }
    }

    font_families_[font] = families;
}
