#pragma once

#include <QString>
#include <QDateTime>
#include <QDate>

class RegistrationCode {
public:
    RegistrationCode(const QByteArray &secret_key);

    enum class ErrorType {
        REGISTRATION_CODE_VALID,            // 注册码验证通过
        REGISTRATION_CODE_INVALID_FORMAT,   // 注册码格式不正确
        REGISTRATION_CODE_EXPIRED,          // 注册码过期
        REGISTRATION_CODE_ABOUT_TO_EXPIRE,  // 注册码将要过期
        REGISTRATION_CODE_INVALID           // 注册码验证不通过
    };

    /*!
     * @brief
     * @return
     */
    QString getUniqueSystemIdentifier() const;

    /*!
     * @brief         生成哈希值
     * 
     * @param         expirationDays
     * @return        QString
     */
    QString generateCode(QString system_identifier, QDate expiration_days) const;

    /*!
     * @brief         
     * 
     * @param         code
     * @return        true
     * @return        false
     * @attention
     */
    ErrorType validateCode(const QString &code);

    QDateTime getEndTime();

private:
    /*!
     * @brief         生成哈希值
     * 
     * @param         expirationDays
     * @return        QString
     */
    QByteArray generateHash(const QString &input) const;

private:
    QByteArray secret_key_;
    QDateTime end_time_;
};
