#pragma once

#include <QString>
#include <QDateTime>
#include <QDate>

class RegistrationCode {
public:
    RegistrationCode(const QByteArray &secretKey);

    enum class ERROT_TYPE {
        RegistrationCodeValid,        // 注册码验证通过
        RegistrationCodeInvalidFormat,// 注册码格式不正确
        RegistrationCodeExpired,      // 注册码过期
        RegistrationCodeAboutToExpire,// 注册码将要过期
        RegistrationCodeInvalid       // 注册码验证不通过
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
    QString generateCode(QString systemIdentifier, QDate expirationDays) const;

    /*!
     * @brief         
     * 
     * @param         code
     * @return        true
     * @return        false
     * @attention
     */
    ERROT_TYPE validateCode(const QString &code);

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
    QByteArray secretKey;
    QDateTime m_end_time;
};
