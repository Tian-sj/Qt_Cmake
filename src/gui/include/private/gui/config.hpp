/*!
 * @file          config.hpp
 * @brief         Tool class that provides some internal interfaces
 * @author        Tian_sj
 * @date          2024-12-18
 */
#pragma once

#include "version.h"

#include <RegistrationCode/registrationcode.hpp>
#include <magic_enum/magic_enum.hpp>
#include <QMap>
#include <QHash>
#include <QVector>
#include <QFont>

class QSettings;
class QTranslator;

class Config
{
public:
    static Config& getInstance();

    enum class Language {
        English,
        Chinese
    };

    enum class Font {
        Inter,
        Noto_Sans_SC
    };

    enum class Theme {
        Light,
        Dark
    };

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    /*!
     * @brief         Set the App Runtime object
     *
     * 设置应用程序运行时间
     * 
     * @param         runtime
     * @attention
     */
    void setAppRuntime(QDateTime runtime);

    /*!
     * @brief         Get the App Runtime object
     *
     * 获取应用程序运行时间
     * 
     * @return        QDateTime
     * @attention
     */
    QDateTime getAppRuntime() const;

    /*!
     * @brief         Register resource
     *
     * 注册资源
     * 
     * @param         file
     * @attention
     */
    void registerResource(QString file);

    /*!
     * @brief         Validate code
     *
     * 验证码
     * 
     * @param         code
     * @return        true
     * @return        false
     * @attention
     */
    bool validateCode(const QString& code);

    /*!
     * @brief         Set the Registration Code
     *
     * 设置注册码
     * 
     * @param         code
     * @attention
     */
    void setRegistrationCode(const QString &code) const;

    /*!
     * @brief         Delete registration code
     *
     * 删除注册码
     * 
     * @attention
     */
    void deleteRegistrationCode() const;

    /*!
     * @brief         Get the Registration Code
     *
     * 获取注册码
     * 
     * @return        QString
     * @attention
     */
    QString getRegistrationCode() const;

    /*!
     * @brief         Get the Unique System Identifier
     *
     * 获取唯一系统标识符
     * 
     * @return        QString
     * @attention
     */
    QString getUniqueSystemIdentifier() const;

    /*!
     * @brief         Whether the registration code expires
     *
     * 注册码是否到期
     * 
     * @return        true
     * @return        false
     * @attention
     */
    bool checkExpirationReminder();

    /*!
     * @brief         Get registration code expiration time
     *
     * 获取注册吗到期时间
     * 
     * @return        QDateTime
     * @attention
     */
    QDateTime getEndTime() const;

    /*!
     * @brief         Get the Registration Code Error Type
     *
     * 获取注册码错误类型
     * 
     * @return        RegistrationCode::ERROT_TYPE
     * @attention
     */
    RegistrationCode::ERROT_TYPE getRegistrationCodeErrorType() const;

    /*!
     * @brief         Set the Language
     *
     * 设置语言
     * 
     * @param         language
     * @attention
     */
    void setLanguage(const Language& language);

    /*!
     * @brief         Get the Language
     *
     * 获取语言
     * 
     * @return        Language
     * @attention
     */
    Language getLanguage() const;

    /*!
     * @brief         Set the Theme
     *
     * 设置主题
     * 
     * @param         theme
     * @attention
     */
    void setTheme(const Theme& theme);

    /*!
     * @brief         Get the Theme
     *
     * 获取主题
     * 
     * @return        Theme
     * @attention
     */
    Theme getTheme() const;

    /*!
     * @brief         Get the theme CSS storage path
     *
     * 获取主题CSS存放路径
     * 
     * @return        QString
     * @attention
     */
    QString getThemePath() const;

    /*!
     * @brief         Get the Font
     *
     * 获取字体
     * 
     * @param         primary
     * @param         pointSize
     * @param         weight
     * @return        QFont
     * @attention
     */
    QFont getFont(Font primary, int pointSize = 12, QFont::Weight weight = QFont::Normal);

    /*!
     * @brief         Get the Font Families
     *
     * 获取字体家族
     * 
     * @param         font
     * @return        QStringList
     * @attention
     */
    QStringList getFontFamilies(Font font);

    /*!
     * @brief         Set the Current User Id
     *
     * 设置当前用户ID
     * 
     * @param         user_id
     * @attention
     */
    void setCurrentUserId(const int& user_id);

    /*!
     * @brief         Get the Current User Id
     *
     * 获取当前用户ID
     * 
     * @return        int
     * @attention
     */
    int getCurrentUserId() const;

private:
    Config();
    ~Config();

    /*!
     * @brief         Detecting the expiration of the registration code
     *
     * 检测注册码是否到期
     * 
     * @param         day
     * @param         date_time_1
     * @param         date_time_2
     * @return        true
     * @return        false
     * @attention
     */
    bool isWithinXDays(const int day, const QDateTime& date_time_1, const QDateTime& date_time_2);

    /*!
     * @brief         Get the Translation File Names
     *
     * 获取翻译文件名称
     * 
     * @attention
     */
    void getTranslationFileNames();

    /*!
     * @brief         Font storage path
     *
     * 字体存放路径
     * 
     * @param         font
     * @return        QString
     * @attention
     */
    QString fontDir(Font font);

    /*!
     * @brief         Load program fonts
     *
     * 加载程序字体
     * 
     * @attention
     */
    void loadFonts();

    /*!
     * @brief         Load Font Catalog
     *
     * 加载字体目录
     * 
     * @param         font
     * @param         dir_path
     * @attention
     */
    void loadFontDir(Font font, const QString& dir_path);

private:
    QSettings* m_settings;
    RegistrationCode* m_reg_code;

    QMap<Language, QStringList> map_language_path;
    QVector<QTranslator*> translators;
    RegistrationCode::ERROT_TYPE m_registration_code_error_type;

    QHash<Font, QStringList> font_families;

    QString theme_path;

    int m_user_id;
};
