/*
 * @Author: Tian_sj
 * @Date: 2023-10-18 16:07:35
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-12-12 10:11:49
 * @FilePath: /communication/device/src/device.cpp
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#include <device/device.h>
#include <cache/cachebase.h>
#include <comm/comminterface.h>
#include <comm/serialport.h>
#include <comm/tcpsocket.h>
#include <comm/commprotocol.h>
#include <comm/commmodbusrtu.h>
#include <comm/commmodbustcp.h>

Device::Device(Comm_Type comm_type, Protocol_Type protocol_type) : m_comm_type(comm_type), m_protocol_type(protocol_type)
{
    cache = new CacheBase();

    switch (comm_type)
    {
    case Comm_Type::RS232:
    case Comm_Type::RS485:
    {
        comm_interface = new CommOfRS485();
        index_paramter.insert(std::pair<QString, QString>("串口名称", "PortName"));
        index_paramter.insert(std::pair<QString, QString>("波特率", "BaudRate"));
        index_paramter.insert(std::pair<QString, QString>("数据位", "DataBits"));
        index_paramter.insert(std::pair<QString, QString>("校验位", "Parity"));
        index_paramter.insert(std::pair<QString, QString>("停止位", "StopBits"));
        break;
    }
    case Comm_Type::EthernetTCP:
    {
        comm_interface = new CommOfEthernet();
        index_paramter.insert(std::pair<QString, QString>("IP", "ServerAddress"));
        index_paramter.insert(std::pair<QString, QString>("Port", "ServerPort"));
        break;
    }
    default:
        break;
    }

    switch (protocol_type)
    {
    case Protocol_Type::ModbusRTU:
        comm_protocol = new commModbusRTU();
        break;
    case Protocol_Type::ModbusTCP:
        comm_protocol = new commModbusTCP();
        break;
    default:
        break;
    }

    comm_interface->set_protocol(comm_protocol);
    comm_protocol->set_comm(comm_interface);
    comm_protocol->set_cache(cache);
}

void Device::read(uint16_t address, uint16_t sum)
{
    QByteArray ba;
    switch (m_protocol_type)
    {
    case Protocol_Type::ModbusRTU:
    {
        ba = QByteArray::fromHex(QString("0103'%1''%2'")
                                     .arg(address, 4, 16, QLatin1Char('0'))
                                     .arg(sum, 4, 16, QLatin1Char('0'))
                                     .toLocal8Bit());
        break;
    }
    case Protocol_Type::ModbusTCP:
    {
        ba = QByteArray::fromHex(QString("0000000000060103'%1''%2'")
                                     .arg(address, 4, 16, QLatin1Char('0'))
                                     .arg(sum, 4, 16, QLatin1Char('0'))
                                     .toLocal8Bit());
        break;
    }
    default:
        break;
    }
    comm_interface->send_order(ba);
}

void Device::read(QByteArray ba)
{
    comm_interface->send_order(ba, CommInterface::READ);
}

void Device::send(uint16_t address, uint16_t data)
{
    QByteArray ba;
    switch (m_protocol_type)
    {
    case Protocol_Type::ModbusRTU:
    {
        ba = QByteArray::fromHex(QString("0106'%1''%2'")
                                     .arg(address, 4, 16, QLatin1Char('0'))
                                     .arg(data, 4, 16, QLatin1Char('0'))
                                     .toLocal8Bit());
        break;
    }
    case Protocol_Type::ModbusTCP:
    {
        ba = QByteArray::fromHex(QString("0000000000060106'%1''%2'")
                                     .arg(address, 4, 16, QLatin1Char('0'))
                                     .arg(data, 4, 16, QLatin1Char('0'))
                                     .toLocal8Bit());
        break;
    }
    default:
        break;
    }
    comm_interface->send_order(ba, CommInterface::WRITE);
}

void Device::send(QByteArray ba)
{
    comm_interface->send_order(ba, CommInterface::WRITE);
}

void Device::send_clear()
{
    comm_interface->send_order_clear();
}

void Device::send(uint16_t address, uint16_t sum, QList<uint16_t> data)
{
    QByteArray ba;
    switch (m_protocol_type)
    {
    case Protocol_Type::ModbusRTU:
    {
        ba = QByteArray::fromHex(QString("0110'%1''%2''%3'")
                                     .arg(address, 4, 16, QLatin1Char('0'))
                                     .arg(sum, 4, 16, QLatin1Char('0'))
                                     .arg(sum * 2, 2, 16, QLatin1Char('0'))
                                     .toLocal8Bit());

        for (auto it = data.cbegin(); it != data.cend(); ++it)
            ba += QByteArray::fromHex(QString("%1").arg(*it, 4, 16, QLatin1Char('0')).toLocal8Bit());
        break;
    }
    case Protocol_Type::ModbusTCP:
    {
        ba = QByteArray::fromHex(QString("0110'%1''%2''%3'")
                                     .arg(address, 4, 16, QLatin1Char('0'))
                                     .arg(sum, 4, 16, QLatin1Char('0'))
                                     .arg(sum * 2, 2, 16, QLatin1Char('0'))
                                     .toLocal8Bit());

        for (auto it = data.cbegin(); it != data.cend(); ++it)
            ba += QByteArray::fromHex(QString("%1").arg(*it, 4, 16, QLatin1Char('0')).toLocal8Bit());
        break;
        QByteArray b = QByteArray::fromHex(QString("00000000'%1'").arg(ba.size(), 4, 16, QLatin1Char('0')).toLocal8Bit());
        ba = b + ba;
    }
    default:
        break;
    }
    comm_interface->send_order(ba, CommInterface::WRITE);
}

void Device::set_send_delay(int delay)
{
    comm_interface->set_delay(delay);
}

void Device::set_paramter(const QString para_name, const QVariant &para_value)
{
    comm_interface->set_paramter(index_paramter[para_name], para_value);
}

void Device::set_crc_type(CRC_Type crc_type)
{
    m_crc_type = crc_type;
    comm_protocol->set_crc_type(static_cast<commProtocol::CRC_Type>(m_crc_type));
}

const QStringList Device::get_portName() const
{
    return CommOfRS485::get_portName();
}

void Device::link()
{
    comm_interface->link_device();
}

void Device::un_link()
{
    comm_interface->unlink_device();
}

std::vector<int> Device::get_cache(uint16_t address_start, uint16_t num)
{
    return cache->get_cache(address_start, num);
}

void Device::registerCallback_updateUI(uint16_t address, TextCallback callback)
{
    cache->registerCallback(address, callback);
}

void Device::registerCallback_updateLink(comm_state_chaged callback)
{
    comm_interface->registerCallback(callback);
}

void Device::registerCallback_send_success(send_success callback)
{
    comm_protocol->registerCallback(callback);
}
