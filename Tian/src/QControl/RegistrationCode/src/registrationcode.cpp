#include <RegistrationCode/registrationcode.h>
#include <QCryptographicHash>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>
#include <QProcess>
#include <QRegularExpression>

namespace {
#ifdef Q_OS_WIN
QString get_motherboard_serial_number_windows() {
    QProcess process;
    process.start("wmic", QStringList() << "baseboard" << "get" << "serialnumber");
    if (!process.waitForFinished()) {
        return QString();
    }

    process.readLine(); // 丢弃第一行，因为第一行是标题
    QString serialNumber = process.readLine(); // 读取第二行并去除空白字符
    QRegularExpression re("[^a-zA-Z0-9]");

    return serialNumber.replace(re, QString(""));
}
#endif

#ifdef Q_OS_LINUX
QString get_motherboard_serial_number_linux() {
    QFile file("/sys/class/dmi/id/board_serial");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    QString serial_number = file.readLine().trimmed();
    file.close();
    return serial_number;
}
#endif

#ifdef Q_OS_MAC
QString get_motherboard_serial_number_mac() {
    QProcess process;
    process.start("bash", QStringList() << "-c" << "system_profiler SPHardwareDataType | awk '/Serial Number/ {print $4}'");
    if (!process.waitForFinished()) {
        return QString();
    }

    QByteArray output = process.readAllStandardOutput();
    QString serialNumber(output.trimmed());

    return serialNumber;
}
#endif
}

RegistrationCode::RegistrationCode(const QByteArray &secretKey)
    : secretKey(secretKey), localFile("registration_info.json") {}

QString RegistrationCode::getUniqueSystemIdentifier() const
{
    QString osType = QSysInfo::productType();

#ifdef Q_OS_WIN
    return get_motherboard_serial_number_windows();
#endif
#ifdef Q_OS_LINUX
    return get_motherboard_serial_number_linux();
#endif
#ifdef Q_OS_MAC
    return get_motherboard_serial_number_mac();
#endif

    return QString();
}

QByteArray RegistrationCode::generateHash(const QString &input) const
{
    QByteArray data = input.toUtf8() + secretKey;
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}

QString RegistrationCode::generateCode(QString systemIdentifier, int expirationDays) const
{
    if (systemIdentifier.isEmpty())
    {
        qWarning() << "Failed to get unique system identifier.";
        return QString();
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    QDateTime expirationTime = currentTime.addDays(expirationDays);
    QString data = QString("%1|%2|%3").arg(systemIdentifier).arg(currentTime.toSecsSinceEpoch()).arg(expirationTime.toSecsSinceEpoch());
    QByteArray hash = generateHash(data);
    return QString("%1|%2").arg(data).arg(QString(hash));
}

bool RegistrationCode::validateCode(const QString &code) const
{
    QStringList parts = code.split('|');
    if (parts.size() != 4)
    {
        return false;
    }

    QString uniqueIdentifier = parts[0];

    if (uniqueIdentifier != getUniqueSystemIdentifier()) {
        return false;
    }

    qint64 startTime = parts[1].toLongLong();
    qint64 expirationTime = parts[2].toLongLong();
    QByteArray hash = parts[3].toUtf8();


    QString data = QString("%1|%2|%3").arg(uniqueIdentifier).arg(startTime).arg(expirationTime);
    QByteArray expectedHash = generateHash(data);

    if (hash != expectedHash)
    {
        return false;
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    if (currentTime.toSecsSinceEpoch() > expirationTime)
    {
        return false;
    }

    return true;
}

void RegistrationCode::storeCode(const QString &code) const
{
    QFile file(localFile);
    if (file.open(QIODevice::WriteOnly))
    {
        QJsonObject json;
        json["registration_code"] = code;
        json["stored_time"] = QDateTime::currentDateTime().toSecsSinceEpoch();
        file.write(QJsonDocument(json).toJson());
    }
}

QJsonObject RegistrationCode::loadCode() const
{
    QFile file(localFile);
    if (file.open(QIODevice::ReadOnly))
    {
        return QJsonDocument::fromJson(file.readAll()).object();
    }
    return QJsonObject();
}
