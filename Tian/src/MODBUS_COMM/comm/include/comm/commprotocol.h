/*
 * @Author: Tian_sj
 * @Date: 2023-10-10 09:17:57
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-10-31 18:24:27
 * @FilePath: /communication/comm/include/comm/commprotocol.h
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#ifndef COMMPROTOCOL_H
#define COMMPROTOCOL_H

#include <QObject>
#include <QPointer>
#include <thread>
#include <cache/cachebase.h>
#include <comm/MsgQueue.h>

class CommInterface;

class commProtocol : public QObject
{
    Q_OBJECT
public:
    // CRC类型
    enum class CRC_Type
    {
        No_CRC = 0,
        ModBusCRC16
    };

public:
    explicit commProtocol(CRC_Type crc);
    ~commProtocol();

    void push_datas(QByteArray ba);

    void set_comm(QPointer<CommInterface> comm);
    void set_cache(QPointer<CacheBase> cache);

    void clear_receives();

    void set_crc_type(CRC_Type type);

    void open_change(bool open);

    QByteArray add_CRC(QByteArray data);

    using send_success = std::function<void()>;
    void registerCallback(send_success callback);

protected:
    virtual const QByteArray get_datas();

    virtual void analysis_thread_function() = 0;

    QByteArray sub_CRC(QByteArray data);
    bool verify_CRC(QByteArray data);
    int CRC_size();

protected:
    MsgQueue<QByteArray> m_receive_buffer;

    CRC_Type m_crc_type;

    QPointer<CommInterface> m_comm_interface;
    QPointer<CacheBase> m_cache;

    std::thread analysis_thread;
    bool m_open;
    bool analysis_thread_running;

    std::vector<send_success> Callback_vector;
};

#endif // COMMPROTOCOL_H
