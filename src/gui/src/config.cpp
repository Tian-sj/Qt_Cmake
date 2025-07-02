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
    : m_settings(new QSettings(QSettings::NativeFormat, QSettings::UserScope, _DEVELOPER, _NAME))
    , m_reg_code(new RegistrationCode(QString("%1_%2").arg(_DEVELOPER, _NAME).toLocal8Bit())) {
    m_settings->setValue("version", _VERSION);

    getTranslationFileNames();
    loadFonts();
}

Config::~Config() {
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

void Config::setLanguage(const Language &language) {
    auto name_view = magic_enum::enum_name(language);
    m_settings->setValue("Language", QString::fromUtf8(name_view.data(), static_cast<int>(name_view.size())));

    QStringList list = map_language_path[language];
    size_t i { 0 };
    foreach (const auto &file_path, list) {
        qDebug() << file_path << ":" << translators[i]->load(file_path);
        qApp->installTranslator(translators[i++]);
    }
}

Config::Language Config::getLanguage() const {
    auto name = magic_enum::enum_cast<Language>(m_settings->value("Language").toString().toStdString());
    if (name.has_value())
        return name.value();
    else
        return Language::English;
}

void Config::setTheme(const Theme &theme) {
    auto name_view = magic_enum::enum_name(theme);
    QString path = QString::fromUtf8(name_view.data(), name_view.size());
    m_settings->setValue("Theme", path);
    theme_path = ":/GUI/css/" + path + "/";
}

QString Config::getThemePath() const {
    return theme_path;
}

Config::Theme Config::getTheme() const {
    auto name = magic_enum::enum_cast<Theme>(m_settings->value("Theme").toString().toStdString());
    if (name.has_value())
        return name.value();
    else
        return Theme::Light;
}

QFont Config::getFont(Font primary, int pointSize, QFont::Weight weight) {
    if (!font_families.contains(primary) || font_families[primary].isEmpty()) {
        qWarning() << "[FontManager] Font not found for enum:" << static_cast<int>(primary);
        return QFont();
    }

    QString family = font_families[primary].first();
    QFont font(family);
    font.setPointSize(pointSize);
    font.setWeight(weight);
    return font;
}

QStringList Config::getFontFamilies(Font font) {
    return font_families[font];
}

void Config::setCurrentUserId(const int &user_id) {
    this->m_user_id = user_id;
}

int Config::getCurrentUserId() const {
    return this->m_user_id;
}

void Config::setAppRuntime(QDateTime runtime) {
    m_settings->setValue("App_runtime", runtime);
}

QDateTime Config::getAppRuntime() const {
    return m_settings->value("App_runtime").toDateTime();
}

void Config::registerResource(QString file) {
    qDebug() <<"registerResource:" << QResource::registerResource(file);
}

bool Config::validateCode(const QString &code) {
    m_registration_code_error_type = m_reg_code->validateCode(code);
    switch (m_registration_code_error_type) {
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

void Config::setRegistrationCode(const QString& code) const {
    m_settings->setValue("RegistrationCode", code.toLocal8Bit());
}

void Config::deleteRegistrationCode() const {
    m_settings->remove("RegistrationCode");
}

QString Config::getRegistrationCode() const {
    return QString::fromLocal8Bit(m_settings->value("RegistrationCode").toByteArray());
}

QString Config::getUniqueSystemIdentifier() const {
    return m_reg_code->getUniqueSystemIdentifier();
}

bool Config::checkExpirationReminder() {
    if (m_registration_code_error_type == RegistrationCode::ERROT_TYPE::RegistrationCodeAboutToExpire)
        return true;
    
    if (isWithinXDays(3, QDateTime::currentDateTime(), m_reg_code->getEndTime())) {
        m_registration_code_error_type = RegistrationCode::ERROT_TYPE::RegistrationCodeAboutToExpire;
        return true;
    }

    return false;
}

QDateTime Config::getEndTime() const {
    return m_reg_code->getEndTime();
}

RegistrationCode::ERROT_TYPE Config::getRegistrationCodeErrorType() const {
    return m_registration_code_error_type;
}

bool Config::isWithinXDays(const int day, const QDateTime& date_time_1, const QDateTime& date_time_2) {
    int days = qAbs(date_time_1.daysTo(date_time_2));
    return days < day;
}

void Config::getTranslationFileNames() {
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

QString Config::fontDir(Font font) {
    switch (font) {
    case Font::Inter: return QCoreApplication::applicationDirPath() + "/fonts/Inter";
    case Font::Noto_Sans_SC : return QCoreApplication::applicationDirPath() + "/fonts/Noto_Sans_SC";
    default: return "";
    }
}

void Config::loadFonts() {
    loadFontDir(Font::Inter, fontDir(Font::Inter));
    loadFontDir(Font::Noto_Sans_SC, fontDir(Font::Noto_Sans_SC));
}

void Config::loadFontDir(Font font, const QString &dir_path) {
    QStringList families;

    // 遍历资源目录下的所有 .ttf / .otf 字体文件
    QDirIterator it(dir_path, QStringList() << "*.ttf" << "*.otf", QDir::Files);
    while (it.hasNext()) {
        QString filePath = it.next();
        int id = QFontDatabase::addApplicationFont(filePath);
        if (id >= 0) {
            QStringList loaded = QFontDatabase::applicationFontFamilies(id);
            if (!loaded.isEmpty()) {
                families << loaded.first();
                qDebug() << "[FontManager] Loaded:" << filePath << "->" << loaded.first();
            }
        } else {
            qWarning() << "[FontManager] Failed to load:" << filePath;
        }
    }

    font_families[font] = families;
}
