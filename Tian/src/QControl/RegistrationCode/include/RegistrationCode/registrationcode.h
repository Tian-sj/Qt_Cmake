#ifndef REGISTRATIONCODE_H
#define REGISTRATIONCODE_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class RegistrationCode {
public:
    RegistrationCode(const QByteArray &secretKey);

    QString getUniqueSystemIdentifier() const;

    /*!
     * @brief         生成哈希值
     * 
     * @param         expirationDays
     * @return        QString
     */
    QString generateCode(QString systemIdentifier, int expirationDays) const;

    /*!
     * @brief         
     * 
     * @param         code
     * @return        true
     * @return        false
     * @attention
     */
    bool validateCode(const QString &code) const;
    void storeCode(const QString &code) const;
    QJsonObject loadCode() const;

private:
    /*!
     * @brief         生成哈希值
     * 
     * @param         expirationDays
     * @return        QString
     */
    QByteArray generateHash(const QString &input) const;
    QByteArray secretKey;
    QString localFile;
};

#endif // REGISTRATIONCODE_H
