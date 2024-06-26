/*
 * @Author: error: git config user.name & please set dead value or install git
 * @Date: 2023-09-18 12:32:00
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-01 13:11:32
 * @FilePath: /communication/comm/src/comminterface.cpp
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#include <comm/comminterface.h>
#include <comm/commprotocol.h>
#include <iostream>
#include <algorithm>

CommInterface::CommInterface()
    : QObject(), m_delay(200), m_open(false), m_send_order(100), m_read_order(100)
{
    m_sender = new QTimer;
    m_sender->setInterval(m_delay);
    connect(m_sender, &QTimer::timeout, this, &CommInterface::on_send_time);
}

CommInterface::~CommInterface()
{
    delete m_sender;
}

bool CommInterface::get_paramter(const QString para_name, QVariant &para_value)
{
    auto it = m_comm_paras.constFind(para_name);
    if (it != m_comm_paras.constEnd())
    {
        para_value = it.value();
        return true;
    }
    return false;
}

bool CommInterface::set_paramter(const QString para_name, const QVariant &para_value)
{
    m_comm_paras[para_name] = para_value;

    return true;
}

bool CommInterface::is_open() const
{
    return m_open;
}

void CommInterface::set_delay(int delay)
{
    m_delay = delay;
    if (m_sender->isActive())
    {
        m_sender->stop();
        m_sender->setInterval(m_delay);
        m_sender->start();
    }
    m_sender->setInterval(m_delay);
}

int CommInterface::delay() const
{
    return m_delay;
}

void CommInterface::link_device()
{
    open();
}

void CommInterface::unlink_device()
{
    m_sender->stop();
    m_open = false;

    close();
    m_read_order.clear();
    m_send_order.clear();
}

void CommInterface::send_order(QByteArray data, Send_Type send_type)
{
    data = m_protocol->add_CRC(data);
    if (send_type == Send_Type::READ)
    {
        m_read_order.push(std::move(data));
    }
    else if (send_type == Send_Type::WRITE)
    {
        m_send_order.push(std::move(data));
    }
}

bool CommInterface::has_order(Send_Type send_type = Send_Type::READ)
{
    return (send_type == Send_Type::READ) ? !m_read_order.empty() : !m_send_order.empty();
}

void CommInterface::send_order_clear()
{
    m_send_order.clear();
}

void CommInterface::onReceived(QByteArray datas)
{
    m_protocol->push_datas(datas);
}

void CommInterface::set_open(bool open)
{
    m_open = open;

    if (m_open)
    {
        comm_start();
    }

    m_protocol->open_change(open);
    try
    {
        for (auto it = Callback_vector.begin(); it != Callback_vector.end(); ++it)
            if (*it)
                (*it)(m_open);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << ":"
                  << "Callback function not registered!" << '\n';
    }
}

bool CommInterface::get_next_order(QByteArray &data)
{
    if (m_send_order.front(data))
    {
        return true;
    }
    else if (m_read_order.front(data))
    {
        return true;
    }

    return false;
}

void CommInterface::registerCallback(comm_state_chaged callback)
{
    Callback_vector.emplace_back(callback);
}

void CommInterface::comm_start()
{
    m_sender->start();
}

void CommInterface::on_send_time()
{
    QByteArray ord;

    if (get_next_order(ord))
    {
        send_data(ord);
    }
}

void CommInterface::set_protocol(QPointer<commProtocol> protocol)
{
    m_protocol = protocol;
}
