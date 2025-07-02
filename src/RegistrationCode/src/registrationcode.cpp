#include "RegistrationCode/registrationcode.hpp"

#include <QCryptographicHash>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QRegularExpression>
#include <QTime>

RegistrationCode::RegistrationCode(const QByteArray &secretKey)
    : secretKey(secretKey)
{

}

QString RegistrationCode::getUniqueSystemIdentifier() const
{
#ifdef Q_OS_WIN
    QProcess process;
    process.start("wmic", QStringList() << "baseboard" << "get" << "serialnumber");
    if (!process.waitForFinished()) {
        process.start("powershell", QStringList() << "-Command" << "(Get-WmiObject -Class Win32_BaseBoard).SerialNumber");
        if (!process.waitForFinished()) {
            return QString();
        }
    } else {
        process.readLine(); // 丢弃第一行，因为第一行是标题
    }

    QString serialNumber = process.readLine(); // 读取第二行并去除空白字符
    QRegularExpression re("[^a-zA-Z0-9]");

    return serialNumber.replace(re, QString(""));
#endif
#ifdef Q_OS_LINUX
    QFile file("/sys/class/dmi/id/board_serial");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    QString serial_number = file.readLine().trimmed();
    file.close();
    return serial_number;
#endif
#ifdef Q_OS_MAC
    QProcess process;
    process.start("bash", QStringList() << "-c" << "system_profiler SPHardwareDataType | awk '/Serial Number/ {print $4}'");
    if (!process.waitForFinished()) {
        return QString();
    }

    QByteArray output = process.readAllStandardOutput();
    QString serialNumber(output.trimmed());

    return serialNumber;
#endif

    return QString();
}

QByteArray RegistrationCode::generateHash(const QString &input) const
{
    QByteArray data = input.toUtf8() + secretKey;
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}

QString RegistrationCode::generateCode(QString systemIdentifier, QDate expirationDays) const
{
    if (systemIdentifier.isEmpty())
    {
        qWarning() << "Failed to get unique system identifier.";
        return QString();
    }

    QTime t(0, 0, 0);

    QDateTime end_time(expirationDays, t);


    QString data = QString("%1|%2").arg(systemIdentifier).arg(end_time.toSecsSinceEpoch());
    QByteArray hash = generateHash(data);
    return QString("%1|%2").arg(data).arg(QString(hash));
}

RegistrationCode::ERROT_TYPE RegistrationCode::validateCode(const QString &code)
{
    QStringList parts = code.split('|');
    if (parts.size() != 3)
    {
        return ERROT_TYPE::RegistrationCodeInvalidFormat;
    }

    QString uniqueIdentifier = parts[0];

    if (uniqueIdentifier != getUniqueSystemIdentifier()) {
        return ERROT_TYPE::RegistrationCodeInvalid;
    }

    qint64 expirationTime = parts[1].toLongLong();
    QByteArray hash = parts[2].toUtf8();

    m_end_time = QDateTime::fromSecsSinceEpoch(expirationTime);

    QString data = QString("%1|%2").arg(uniqueIdentifier).arg(expirationTime);
    QByteArray expectedHash = generateHash(data);

    if (hash != expectedHash)
    {
        return ERROT_TYPE::RegistrationCodeInvalid;
    }

    QDateTime currentTime = QDateTime::currentDateTime();

    if (currentTime.toSecsSinceEpoch() > expirationTime) {
        return ERROT_TYPE::RegistrationCodeExpired;
    }

    qint64 secondsInThreeDays = 3 * 24 * 60 * 60; // 3 days in seconds
    if (currentTime.toSecsSinceEpoch() < expirationTime && (expirationTime - currentTime.toSecsSinceEpoch()) <= secondsInThreeDays) {
        return ERROT_TYPE::RegistrationCodeAboutToExpire;
    }

    return ERROT_TYPE::RegistrationCodeValid;
}

QDateTime RegistrationCode::getEndTime()
{
    return m_end_time;
}
