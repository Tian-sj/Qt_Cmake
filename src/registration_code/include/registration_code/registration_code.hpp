#pragma once

#include <QString>
#include <QDateTime>
#include <QDate>

/*!
 * @brief         注册码生成与验证工具类
 *
 * Registration code generation and validation utility
 *
 * @attention     用于生成基于系统标识与过期时间的注册码，并进行有效性验证。
 *                Provides functionality to generate and verify registration codes with expiration.
 */
class RegistrationCode {
public:
    /*!
     * @brief         构造函数，设置加密密钥
     *
     * @param         secret_key   用于哈希生成的密钥
     */
    RegistrationCode(const QByteArray &secret_key);

    /*!
     * @brief         注册码验证结果类型
     *
     * Possible results of code validation
     */
    enum class ErrorType {
        REGISTRATION_CODE_VALID,            //!< 注册码验证通过
        REGISTRATION_CODE_INVALID_FORMAT,   //!< 注册码格式不正确
        REGISTRATION_CODE_EXPIRED,          //!< 注册码已过期
        REGISTRATION_CODE_ABOUT_TO_EXPIRE,  //!< 注册码即将过期
        REGISTRATION_CODE_INVALID           //!< 注册码验证失败（内容错误）
    };

    /*!
     * @brief         获取唯一的系统标识符
     *
     * 获取当前系统的唯一 ID（如硬件信息等）
     *
     * @return        唯一系统标识符（字符串）
     */
    QString getUniqueSystemIdentifier() const;

    /*!
     * @brief         生成注册码
     *
     * Generate registration code based on system identifier and expiration date.
     *
     * @param         system_identifier   系统唯一标识符
     * @param         expiration_days     到期日期
     * @return        生成的注册码字符串
     */
    QString generateCode(QString system_identifier, QDate expiration_days) const;

    /*!
     * @brief         验证注册码合法性
     *
     * 验证输入的注册码是否有效，包括格式、过期时间及签名校验
     *
     * @param         code   输入的注册码字符串
     * @return        ErrorType 枚举值，表示验证结果
     *
     * @attention     验证成功会更新内部保存的过期时间
     */
    ErrorType validateCode(const QString &code);

    /*!
     * @brief         获取当前已验证成功的过期时间
     *
     * @return        过期时间（QDateTime 类型）
     */
    QDateTime getEndTime();

private:
    /*!
     * @brief         根据输入生成哈希值
     *
     * 生成用于验证的加密哈希，用于签名校验
     *
     * @param         input   原始输入字符串
     * @return        哈希后的二进制数据
     */
    QByteArray generateHash(const QString &input) const;

private:
    QByteArray secret_key_;  //!< 用于哈希运算的密钥
    QDateTime end_time_;     //!< 验证通过后的有效期截止时间
};
