/*!
 * @file          config.h
 * @brief         Tool class that provides some internal interfaces
 * @author        Tian_sj
 * @date          2024-12-18
 */

#ifndef CHUANQIHMICONFIG_H
#define CHUANQIHMICONFIG_H

#include "version.h"

#include <RegistrationCode/registrationcode.h>
#include <magic_enum/magic_enum.hpp>
#include <QMap>
#include <QVector>

class QSettings;
class QTranslator;

class Config
{
public:
    static Config& get_instance();

    enum class Language {
        English,
        Chinese
    };

    enum class Font {
        Noto_Serif,
        Noto_Sans_SC
    };

    enum class Theme {
        Light,
        Dark
    };

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    void set_app_runtime(QDateTime runtime);
    QDateTime get_app_runtime() const;

    void set_font(const Font& font);
    void display_font_families();

    void registerResource(QString file);
    bool validate_code(const QString& code);
    void set_registration_code(const QString &code) const;
    void delete_registration_code() const;
    QString get_registration_code() const;
    QString get_unique_system_identifier() const;

    bool check_expiration_reminder();
    QDateTime get_end_time() const;

    RegistrationCode::ERROT_TYPE get_registration_code_error_type() const;

    void set_language(const Language& language);
    Language get_language() const;

    void set_theme(const Theme& theme);
    Theme get_theme() const;
    QString get_theme_path() const;

    void set_current_user_id(const int& user_id);
    int get_current_user_id() const;

private:
    Config();
    ~Config();

    bool is_within_x_days(const int day, const QDateTime& date_time_1, const QDateTime& date_time_2);

    void get_translation_file_names();

    void add_font();

private:
    QSettings* m_settings;
    RegistrationCode* m_reg_code;

    QMap<Language, QStringList> map_language_path;
    QVector<QTranslator*> translators;
    RegistrationCode::ERROT_TYPE m_registration_code_error_type;

    QMap<Font, QString> map_font_name;

    QString theme_path;

    int m_user_id;
};

#endif // CHUANQIHMICONFIG_H
