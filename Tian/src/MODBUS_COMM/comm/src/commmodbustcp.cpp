/*
 * @Author: Tian_sj
 * @Date: 2023-10-13 11:47:31
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-01 13:11:44
 * @FilePath: /communication/comm/src/commmodbustcp.cpp
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#include <comm/commmodbustcp.h>
#include <comm/comminterface.h>

commModbusTCP::commModbusTCP() : commProtocol(CRC_Type::No_CRC)
{
}

commModbusTCP::~commModbusTCP()
{
}

void commModbusTCP::analysis_thread_function()
{
    while (true)
    {
        if (!analysis_thread_running)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        if (!m_open)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        QByteArray data = get_datas();
        if (data.size() < 11 || (data.at(5) != data.size() - 6))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        data = data.mid(6, data.size() - 6);
        switch (data.at(1))
        {
        case FunctionCode::WriteSingleRegister:
        {
            QByteArray ba;
            m_comm_interface->m_send_order.front(ba);
            ba = ba.mid(6, ba.size() - 6);
            if (ba == data)
                m_comm_interface->m_send_order.try_pop();
            break;
        }
        case FunctionCode::WriteMultipleRegisters:
        {
            if (data.size() != 6)
                break;
            QByteArray ba;
            m_comm_interface->m_send_order.front(ba);
            ba = ba.mid(6, ba.size() - 6);
            if (ba.left(6) == data.left(6))
                m_comm_interface->m_send_order.try_pop();
            break;
        }
        case FunctionCode::ReadHoldingRegisters:
        {
            QByteArray ba;
            if (!m_comm_interface->m_read_order.front(ba))
                break;
            ba = ba.mid(6, ba.size() - 6);
            uint16_t address = static_cast<uchar>(ba[2]) << 8 | static_cast<uchar>(ba[3]);
            uint16_t sum = static_cast<uchar>(ba[4]) << 8 | static_cast<uchar>(ba[5]);
            uint8_t char_sum = static_cast<uchar>(data[2]);
            if (sum * 2 != char_sum)
                break;
            if (data.size() != (2 + 1 + char_sum))
                break;
            QList<uint16_t> recevie_buffer;
            for (auto it = data.cbegin() + 3; it != data.end(); ++it)
            {
                recevie_buffer << (static_cast<uchar>(*it++) << 8 | static_cast<uchar>(*it));
            }
            m_comm_interface->m_read_order.try_pop();
            m_cache->update_model(address, sum, recevie_buffer);
            break;
        }

        default:
            break;
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
