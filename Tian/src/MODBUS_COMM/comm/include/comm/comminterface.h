/*
 * @Author: error: git config user.name & please set dead value or install git
 * @Date: 2023-09-18 12:32:00
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-10-31 17:12:03
 * @FilePath: /communication/comm/include/comm/comminterface.h
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#ifndef COMMINTERFACE_H
#define COMMINTERFACE_H

#include <QObject>
#include <QByteArray>
#include <QVariant>
#include <QHash>
#include <QList>
#include <QMutex>
#include <QSemaphore>
#include <QThread>
#include <QTimer>
#include <QPointer>
#include <vector>
#include <comm/MsgQueue.h>

class commProtocol;

class CommInterface : public QObject
{
    Q_OBJECT
public:
    using comm_state_chaged = std::function<void(const bool &)>;

    enum Send_Type
    {
        READ = 0,
        WRITE
    };

    explicit CommInterface();
    ~CommInterface();

    virtual bool open() = 0;
    virtual bool close() = 0;

    virtual bool get_paramter(const QString para_name, QVariant &para_value);
    virtual bool set_paramter(const QString para_name, const QVariant &para_value);

    void set_protocol(QPointer<commProtocol> protocol);

    bool is_open() const;

    void set_delay(int delay);
    int delay() const;

    void link_device();
    void unlink_device();

    void send_order(QByteArray data, Send_Type send_type = Send_Type::READ);
    bool has_order(Send_Type send_type);

    void send_order_clear();

    void onReceived(QByteArray datas);

    void registerCallback(comm_state_chaged callback);

protected:
    void set_open(bool open);
    bool get_next_order(QByteArray &data);

    virtual int send_data(QByteArray order) = 0;

private slots:
    void on_send_time();

private:
    void comm_start();

protected:
    QHash<QString, QVariant> m_comm_paras;

    QPointer<commProtocol> m_protocol;

    int m_delay; // 延迟
    bool m_open; // 打开状态

    QTimer *m_sender; // 发送定时

    std::vector<comm_state_chaged> Callback_vector;

public:
    MsgQueue<QByteArray> m_send_order;
    MsgQueue<QByteArray> m_read_order;
};

#endif // COMMINTERFACE_H
