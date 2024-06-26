/*
 * @Author: Tian_sj
 * @Date: 2023-10-10 09:17:57
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-01 13:11:50
 * @FilePath: /communication/comm/src/commprotocol.cpp
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#include <comm/commprotocol.h>

commProtocol::commProtocol(CRC_Type crc) : m_crc_type(crc), m_receive_buffer(100)
{
    analysis_thread_running = false;
    analysis_thread = std::thread(&commProtocol::analysis_thread_function, this);
}

commProtocol::~commProtocol()
{
    analysis_thread_running = false;
    if (analysis_thread.joinable())
    {
        analysis_thread.join();
    }
}

void commProtocol::push_datas(QByteArray ba)
{
    m_receive_buffer.push(std::move(ba));
}

void commProtocol::set_comm(QPointer<CommInterface> comm)
{
    m_comm_interface = comm;
}

void commProtocol::set_cache(QPointer<CacheBase> cache)
{
    m_cache = cache;
}

void commProtocol::clear_receives()
{
    m_receive_buffer.clear();
}

void commProtocol::set_crc_type(CRC_Type type)
{
    m_crc_type = type;
}

const QByteArray commProtocol::get_datas()
{
    return m_receive_buffer.pop();
}

void commProtocol::open_change(bool open)
{

    m_open = std::move(open);
    if (m_open)
    {
        analysis_thread_running = true;
    }
    else
    {
        analysis_thread_running = false;
        m_cache->cleaup_view();
    }
}

QByteArray commProtocol::add_CRC(QByteArray data)
{
    switch (m_crc_type)
    {
    case CRC_Type::No_CRC:
        break;

    case CRC_Type::ModBusCRC16:
    {
        int len = data.size();
        quint16 wcrc = 0xFFFF;
        quint8 temp;
        int i = 0, j = 0;
        for (int i = 0; i < len; ++i)
        {
            temp = data.at(i);
            wcrc ^= temp;
            for (int j = 0; j < 8; ++j)
            {
                if (wcrc & 0x0001)
                {
                    wcrc >>= 1;
                    wcrc ^= 0xA001;
                }
                else
                    wcrc >>= 1;
            }
        }
        wcrc = (wcrc << 8) | (wcrc >> 8);
        data.append(QByteArray::fromHex(QString("%1").arg(wcrc, 4, 16, QLatin1Char('0')).toLocal8Bit()));
        break;
    }

    default:
        break;
    }
    return data;
}

QByteArray commProtocol::sub_CRC(QByteArray data)
{
    switch (m_crc_type)
    {
    case CRC_Type::No_CRC:
        break;

    case CRC_Type::ModBusCRC16:
    {
        data = data.left(data.size() - 2);
        break;
    }

    default:
        break;
    }
    return data;
}

bool commProtocol::verify_CRC(QByteArray data)
{
    bool verify = false;
    switch (m_crc_type)
    {
    case CRC_Type::No_CRC:
    {
        verify = true;
        break;
    }
    case CRC_Type::ModBusCRC16:
    {
        QByteArray crc = data.right(2);
        data = data.left(data.size() - 2);
        int len = data.size();
        quint16 wcrc = 0xFFFF;
        quint8 temp;
        int i = 0, j = 0;
        for (int i = 0; i < len; ++i)
        {
            temp = data.at(i);
            wcrc ^= temp;
            for (int j = 0; j < 8; ++j)
            {
                if (wcrc & 0x0001)
                {
                    wcrc >>= 1;
                    wcrc ^= 0xA001;
                }
                else
                    wcrc >>= 1;
            }
        }
        wcrc = (wcrc << 8) | (wcrc >> 8);
        data.clear();
        data.append(QByteArray::fromHex(QString("%1").arg(wcrc, 4, 16, QLatin1Char('0')).toLocal8Bit()));
        verify = !data.compare(crc);
        break;
    }

    default:
        break;
    }
    return verify;
}

int commProtocol::CRC_size()
{
    int size;
    switch (m_crc_type)
    {
    case CRC_Type::No_CRC:
        size = 0;
        break;
    case CRC_Type::ModBusCRC16:
        size = 2;
        break;
    default:
        break;
    }
    return size;
}

void commProtocol::registerCallback(send_success callback)
{
    Callback_vector.emplace_back(callback);
}
