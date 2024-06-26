#include <comm/serialport.h>

SerialPort::SerialPort(CommOfRS485 *comm)
{
    qRegisterMetaType<QSerialPort::SerialPortError>("QSerialPort::SerialPortError");
    m_comm = comm;
    m_port = nullptr;
}

SerialPort::~SerialPort()
{
}

bool SerialPort::open_port()
{
    QVariant serial_name, serial_baud, serial_data, serial_parity, serial_stop;
    if (!m_comm->get_paramter("PortName", serial_name) ||
        !m_comm->get_paramter("BaudRate", serial_baud) ||
        !m_comm->get_paramter("DataBits", serial_data) ||
        !m_comm->get_paramter("Parity", serial_parity) ||
        !m_comm->get_paramter("StopBits", serial_stop))
    {
        return false;
    }

    close_port();

    m_port = new QSerialPort();
    connect(m_port, &QSerialPort::readyRead, this, &SerialPort::handle_data);
    connect(m_port, &QSerialPort::errorOccurred, m_comm, &CommOfRS485::handleSerialError);
    connect(this, &SerialPort::emit_open, m_comm, &CommOfRS485::on_open);

    // 设置串口名称
    m_port->setPortName(serial_name.toString());
    // 设置波特率
    m_port->setBaudRate(serial_baud.toInt());
    // 设置数据位
    m_port->setDataBits(serial_data.value<QSerialPort::DataBits>());
    // 设置校验位
    m_port->setParity(serial_parity.value<QSerialPort::Parity>());
    // 设置停止位
    m_port->setStopBits(serial_stop.value<QSerialPort::StopBits>());

    if (m_port->open(QIODevice::ReadWrite))
        emit emit_open(true);
    else
        emit emit_open(false);

    return true;
}

void SerialPort::close_port()
{
    if (m_port != nullptr)
    {
        disconnect(m_port, nullptr, nullptr, nullptr);
        m_port->close();
        delete m_port;
        m_port = nullptr;
    }
}

void SerialPort::on_start()
{
    if (!open_port())
        return;
}
void SerialPort::on_stop()
{
    m_port->disconnect();
}

void SerialPort::handle_data()
{
    QByteArray receive = m_port->readAll();

    m_comm->onReceived(receive);
}

void SerialPort::write_data(const QByteArray data)
{
    m_port->write(data);
    m_port->flush();
    m_port->waitForBytesWritten(10);
}

CommOfRS485::CommOfRS485() : CommInterface()
{
    m_worker = new SerialPort(this);
    m_thread = new QThread(this);
    m_worker->moveToThread(m_thread);
    connect(this, &CommOfRS485::start_running, m_worker, &SerialPort::on_start);
    connect(this, &CommOfRS485::write_data, m_worker, &SerialPort::write_data);
}

CommOfRS485::~CommOfRS485()
{
    if (is_open())
    {
        close();
    }
    m_worker->deleteLater();
}

bool CommOfRS485::open()
{
    m_thread->start();
    emit start_running();

    return true;
}

bool CommOfRS485::close()
{
    m_worker->on_stop();
    m_thread->quit();
    m_thread->wait();

    set_open(false);

    return true;
}

QStringList CommOfRS485::get_portName()
{
    QStringList portName;
    QSerialPort temp_serial;
    foreach (const QSerialPortInfo &Info, QSerialPortInfo::availablePorts())
    {
        temp_serial.setPort(Info);
        if (temp_serial.open(QIODevice::ReadWrite))
        {
            portName << Info.portName();
            temp_serial.close();
        }
    }
    return portName;
}

int CommOfRS485::send_data(QByteArray order)
{
    emit write_data(order);
    return order.size();
}

void CommOfRS485::handleSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError || error == QSerialPort::PermissionError)
    {
        set_open(false);
    }
}

void CommOfRS485::on_open(bool state)
{
    set_open(state);
}
