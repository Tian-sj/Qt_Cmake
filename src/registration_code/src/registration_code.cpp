#include "registration_code/registration_code.hpp"

#include <QCryptographicHash>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QRegularExpression>
#include <QTime>

RegistrationCode::RegistrationCode(const QByteArray &secret_key)
    : secret_key_(secret_key)
{}

QString RegistrationCode::getUniqueSystemIdentifier() const {
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

    QString serial_number = process.readLine(); // 读取第二行并去除空白字符
    QRegularExpression re("[^a-zA-Z0-9]");

    return serial_number.replace(re, QString(""));
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
    QString serial_number(output.trimmed());

    return serial_number;
#endif

    return QString();
}

QByteArray RegistrationCode::generateHash(const QString &input) const {
    QByteArray data = input.toUtf8() + secret_key_;
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}

QString RegistrationCode::generateCode(QString system_identifier, QDate expiration_days) const {
    if (system_identifier.isEmpty()) {
        qWarning() << "Failed to get unique system identifier.";
        return QString();
    }

    QTime t(0, 0, 0);

    QDateTime end_time(expiration_days, t);


    QString data = QString("%1|%2").arg(system_identifier).arg(end_time.toSecsSinceEpoch());
    QByteArray hash = generateHash(data);
    return QString("%1|%2").arg(data).arg(QString(hash));
}

RegistrationCode::ErrorType RegistrationCode::validateCode(const QString &code) {
    QStringList parts = code.split('|');
    if (parts.size() != 3) {
        return ErrorType::REGISTRATION_CODE_INVALID_FORMAT;
    }

    QString unique_identifier = parts[0];

    if (unique_identifier != getUniqueSystemIdentifier()) {
        return ErrorType::REGISTRATION_CODE_INVALID;
    }

    qint64 expiration_time = parts[1].toLongLong();
    QByteArray hash = parts[2].toUtf8();

    end_time_ = QDateTime::fromSecsSinceEpoch(expiration_time);

    QString data = QString("%1|%2").arg(unique_identifier).arg(expiration_time);
    QByteArray expectedHash = generateHash(data);

    if (hash != expectedHash) {
        return ErrorType::REGISTRATION_CODE_INVALID;
    }

    QDateTime currentTime = QDateTime::currentDateTime();

    if (currentTime.toSecsSinceEpoch() > expiration_time) {
        return ErrorType::REGISTRATION_CODE_EXPIRED;
    }

    qint64 seconds_in_three_days = 3 * 24 * 60 * 60; // 3 days in seconds
    if (currentTime.toSecsSinceEpoch() < expiration_time && (expiration_time - currentTime.toSecsSinceEpoch()) <= seconds_in_three_days) {
        return ErrorType::REGISTRATION_CODE_ABOUT_TO_EXPIRE;
    }

    return ErrorType::REGISTRATION_CODE_VALID;
}

QDateTime RegistrationCode::getEndTime() {
    return end_time_;
}
