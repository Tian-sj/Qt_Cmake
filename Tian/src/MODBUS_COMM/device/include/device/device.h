/*
 * @Author: Tian_sj
 * @Date: 2023-10-18 16:07:29
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-12-12 10:12:07
 * @FilePath: /communication/device/include/device/device.h
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <QByteArray>
#include <QString>
#include <QList>
#include <QVariant>
#include <map>

class CommInterface;
class commProtocol;
class CacheBase;

class Device
{
public:
    enum class Comm_Type
    {
        RS232 = 0,   // RS232通信类型
        RS485,       // RS485通信类型
        EthernetTCP, // 以太网TCP通信类型
        UNKNOW       // 未知通信类型
    };

    // 通讯协议
    enum class Protocol_Type
    {
        ModbusRTU = 0,
        ModbusTCP
    };

    // CRC类型
    enum class CRC_Type
    {
        No_CRC = 0,
        ModBusCRC16
    };

    using TextCallback = std::function<void(const int &)>;
    using comm_state_chaged = std::function<void(const bool &)>;
    using send_success = std::function<void()>;

public:
    Device(Comm_Type comm_type, Protocol_Type protocol_type);
    ~Device() = default;

    void read(uint16_t address, uint16_t sum);
    void read(QByteArray ba);
    void send(uint16_t address, uint16_t data);
    void send(uint16_t address, uint16_t sum, QList<uint16_t> data);
    void send(QByteArray ba);

    void send_clear();

    void set_send_delay(int delay);

    void set_paramter(const QString para_name, const QVariant &para_value);
    void set_crc_type(CRC_Type crc_type);

    const QStringList get_portName() const;

    void link();
    void un_link();

    std::vector<int> get_cache(uint16_t address_start, uint16_t num);

    // 回调函数
public:
    void registerCallback_updateUI(uint16_t address, TextCallback callback);
    void registerCallback_updateLink(comm_state_chaged callback);
    void registerCallback_send_success(send_success callback);

private:
    std::map<QString, QString> index_paramter;

    CacheBase *cache;
    CommInterface *comm_interface;
    commProtocol *comm_protocol;

    Comm_Type m_comm_type;
    Protocol_Type m_protocol_type;
    CRC_Type m_crc_type;
};

#endif // DEVICE_H
