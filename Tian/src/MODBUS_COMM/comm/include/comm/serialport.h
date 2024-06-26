/*
 * @Author: Tian_sj
 * @Date: 2023-09-22 19:42:36
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-08 12:09:35
 * @FilePath: /communication/comm/include/comm/serialport.h
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <comm/comminterface.h>

#include <QSerialPort>
#include <QSerialPortInfo>

class CommOfRS485;

class SerialPort : public QObject
{
    Q_OBJECT
public:
    SerialPort(CommOfRS485 *comm);
    ~SerialPort();

public slots:
    void on_start();
    void on_stop();
    void write_data(const QByteArray data); // 发送数据
    void handle_data();                     // 处理接收到的数据

signals:
    void emit_open(bool state);

private:
    bool open_port();  // 打开串口
    void close_port(); // 关闭串口

private:
    CommOfRS485 *m_comm;
    QSerialPort *m_port; // 串口指针
};

class CommOfRS485 : public CommInterface
{
    Q_OBJECT

public:
    CommOfRS485();
    ~CommOfRS485();

    virtual bool open() override;
    virtual bool close() override;

    static QStringList get_portName();

signals:
    void start_running();
    void write_data(const QByteArray datas);

public slots:
    void handleSerialError(QSerialPort::SerialPortError serialPortError);
    void on_open(bool state);

protected:
    virtual int send_data(QByteArray order) override;

private:
    SerialPort *m_worker;
    QThread *m_thread;
};

#endif // SERIALPORT_H
