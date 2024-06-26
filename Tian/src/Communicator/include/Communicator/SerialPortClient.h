/*!
 * @file          SerialPortClient.h
 * @brief         串口通讯类
 * @author        Tian_sj
 * @date          2024-03-07
 */
#ifndef SERIALPORTCLIENT_H
#define SERIALPORTCLIENT_H

#include <boost/asio.hpp>
#include <thread>
#include <Communicator/CommunicationInterface.h>

#define BYTES_MAX_SIZE 1024

class CommOfSerialPort;

class SerialPortClient
{
public:
    SerialPortClient(CommOfSerialPort *comm);
    ~SerialPortClient();

    /*!
     * @brief         串口打开
     * 
     * @attention
     */
    void open();

    /*!
     * @brief         串口关闭
     * 
     * @attention
     */
    void close();

    /*!
     * @brief         发送消息
     * 
     * @param         buf
     * @attention
     */
    void writeToSocket(const std::vector<char>& buf);

    /*!
     * @brief         接收消息
     * 
     * @attention
     */
    void readToSocket();

private:
    /*!
     * @brief         消息发送触发回调函数，用来判断是否发送成功
     * 
     * @param         ec
     * @param         bytes
     * @attention
     */
    void on_write(const boost::system::error_code& ec, std::size_t bytes);

    /*!
     * @brief         消息接收触发回调函数，用来判断是否接收成功
     * 
     * @param         ec
     * @param         bytes
     * @attention
     */
    void on_read(const boost::system::error_code& ec, std::size_t bytes);

    boost::asio::serial_port::baud_rate get_baud_rate();
    boost::asio::serial_port::parity get_parity();
    boost::asio::serial_port::flow_control get_flow_control();
    boost::asio::serial_port::stop_bits get_stop_bits();
    boost::asio::serial_port::character_size get_character_size();

private:
    CommOfSerialPort *m_comm;
    boost::asio::io_context *io_context_;
    boost::asio::serial_port *serial_port_;
    bool started_;
    std::thread io_context_run;
    std::atomic<bool> stop;
    std::vector<char> buffer_;
    std::vector<char> buf_;

};

class CommOfSerialPort : public CommunicationInterface
{
public:
    CommOfSerialPort(/* args */);
    ~CommOfSerialPort();
};


#endif // SERIALPORTCLIENT_H