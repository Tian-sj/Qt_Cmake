#pragma once

#include "version.h"

#include <registration_code/registration_code.hpp>
#include <QHash>
#include <QVector>
#include <QFont>

class QSettings;
class QTranslator;

/*!
 * @brief         全局配置类（单例模式）
 *
 * Global configuration singleton class
 *
 * @attention     管理程序运行时配置、主题、字体、语言、本地化资源、注册码等。
 */
class Config
{
public:
    /*!
     * @brief         获取唯一实例
     *
     * Get the global singleton instance
     */
    static Config& getInstance();

    /*!
     * @brief         支持的语言
     */
    enum class Language {
        English,  //!< 英文
        Chinese   //!< 中文
    };

    /*!
     * @brief         支持的字体类型
     */
    enum class Font {
        HarmonyOS_Sans,      //!< 英文字体
        HarmonyOS_Sans_SC    //!< 中文字体
    };

    /*!
     * @brief         支持的主题样式
     */
    enum class Theme {
        Light,  //!< 亮色主题
        Dark    //!< 暗色主题
    };

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    /*!
     * @brief         设置应用启动时间
     *
     * @param         runtime   QDateTime 对象
     */
    void setAppRuntime(QDateTime runtime);

    /*!
     * @brief         获取应用启动时间
     *
     * @return        QDateTime 启动时间
     */
    QDateTime getAppRuntime() const;

    /*!
     * @brief         注册 Qt 资源文件（*.qrc）
     *
     * @param         file   资源文件路径
     */
    void registerResource(QString file);

    /*!
     * @brief         验证注册码
     *
     * @param         code   输入注册码
     * @return        true 表示合法，false 表示无效
     */
    bool validateCode(const QString& code);

    /*!
     * @brief         设置注册码（写入配置）
     *
     * @param         code   注册码字符串
     */
    void setRegistrationCode(const QString &code) const;

    /*!
     * @brief         删除已存储的注册码
     */
    void deleteRegistrationCode() const;

    /*!
     * @brief         获取当前存储的注册码
     *
     * @return        注册码字符串
     */
    QString getRegistrationCode() const;

    /*!
     * @brief         获取系统唯一标识符
     *
     * @return        唯一标识符字符串
     */
    QString getUniqueSystemIdentifier() const;

    /*!
     * @brief         检查注册码是否即将过期
     *
     * @return        true 表示即将过期，false 表示未过期
     */
    bool checkExpirationReminder();

    /*!
     * @brief         获取注册码的过期时间
     *
     * @return        QDateTime 对象
     */
    QDateTime getEndTime() const;

    /*!
     * @brief         获取最近一次校验的注册码错误类型
     *
     * @return        RegistrationCode::ErrorType 枚举值
     */
    RegistrationCode::ErrorType getRegistrationCodeErrorType() const;

    /*!
     * @brief         设置当前语言
     *
     * @param         language   语言枚举值
     */
    void setLanguage(const Language& language);

    /*!
     * @brief         获取当前语言
     *
     * @return        当前语言
     */
    Language getLanguage() const;

    /*!
     * @brief         设置当前主题
     *
     * @param         theme   主题枚举值
     */
    void setTheme(const Theme& theme);

    /*!
     * @brief         获取当前主题
     *
     * @return        当前主题
     */
    Theme getTheme() const;

    /*!
     * @brief         获取当前主题的 CSS 路径
     *
     * @return        路径字符串
     */
    QString getThemePath() const;

    /*!
     * @brief         获取字体对象
     *
     * @param         primary     字体类型
     * @param         point_size  字号大小（默认12）
     * @param         weight      字体粗细（默认普通）
     * @return        QFont 对象
     */
    QFont getFont(Font primary, int point_size = 12, QFont::Weight weight = QFont::Normal);

    /*!
     * @brief         获取指定字体类型的字体家族列表
     *
     * @param         font   字体枚举值
     * @return        字体家族字符串列表
     */
    QStringList getFontFamilies(Font font);

    /*!
     * @brief         设置当前用户 ID
     *
     * @param         user_id   整型用户编号
     */
    void setCurrentUserId(const int& user_id);

    /*!
     * @brief         获取当前用户 ID
     *
     * @return        整型 ID
     */
    int getCurrentUserId() const;

private:
    /*!
     * @brief         构造函数（私有，单例模式）
     */
    Config();

    /*!
     * @brief         析构函数
     */
    ~Config();

    /*!
     * @brief         判断两个时间点是否在指定天数内
     *
     * @param         day           天数阈值
     * @param         date_time_1   时间点 1
     * @param         date_time_2   时间点 2
     * @return        true 表示在范围内
     */
    bool isWithinXDays(const int day, const QDateTime& date_time_1, const QDateTime& date_time_2);

    /*!
     * @brief         获取翻译文件名
     */
    void getTranslationFileNames();

    /*!
     * @brief         获取字体路径
     *
     * @param         font   字体枚举值
     * @return        路径字符串
     */
    QString fontDir(Font font);

    /*!
     * @brief         加载所有字体
     */
    void loadFonts();

    /*!
     * @brief         加载指定目录下字体文件
     *
     * @param         font      字体类型
     * @param         dir_path  字体目录路径
     */
    void loadFontDir(Font font, const QString& dir_path);

private:
    QSettings* settings_;                                   //!< 应用配置管理器
    RegistrationCode* reg_code_;                            //!< 注册码处理器

    QHash<Language, QStringList> map_language_path_;        //!< 语言 -> 翻译文件列表
    QVector<QTranslator*> translators_;                     //!< Qt 翻译器对象集合
    RegistrationCode::ErrorType registration_code_error_type_;  //!< 注册码错误类型状态缓存

    QHash<Font, QStringList> font_families_;                //!< 字体 -> 可用家族列表
    QString theme_path_;                                    //!< 当前主题的 CSS 路径
    int user_id_;                                           //!< 当前用户 ID
};
