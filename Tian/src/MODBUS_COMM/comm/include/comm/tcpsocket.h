/*
 * @Author: Tian_sj
 * @Date: 2023-10-18 18:16:41
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-08 11:59:54
 * @FilePath: /communication/comm/include/comm/tcpsocket.h
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
/*
 * @Author: Tian_sj
 * @Date: 2023-09-22 19:42:36
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-08 11:26:48
 * @FilePath: /communication/comm/include/comm/tcpsocket.h
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <comm/comminterface.h>

#include <QTcpSocket>

class CommOfEthernet;

class TcpSocket : public QObject
{
    Q_OBJECT
public:
    TcpSocket(CommOfEthernet *comm);
    ~TcpSocket();

public slots:
    void on_start();
    void on_stop();
    void write_data(const QByteArray data); // 发送数据
    void handle_data();                     // 处理接收到的数据

private:
    bool connect_to_server();      // 打开串口
    void disconnect_from_server(); // 关闭串口

private:
    CommOfEthernet *m_comm;
    QTcpSocket *m_tcp; // 串口指针
};

class CommOfEthernet : public CommInterface
{
    Q_OBJECT

public:
    CommOfEthernet();
    ~CommOfEthernet();

    virtual bool open() override;
    virtual bool close() override;

signals:
    void start_running();
    void write_data(const QByteArray datas);

public slots:
    void onStateChanged(QAbstractSocket::SocketState state);

protected:
    virtual int send_data(QByteArray order) override;

private:
    TcpSocket *m_worker;
    QThread *m_thread;
};

#endif // TCPSOCKET_H
