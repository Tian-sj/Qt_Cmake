/*!
 * @file          TCPClient.h
 * @brief         TCP通讯类
 * @author        Tian_sj
 * @date          2024-03-04
 */
#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <boost/asio.hpp>
#include <thread>
#include <Communicator/CommunicationInterface.h>

#define BYTES_MAX_SIZE 1024

class CommOfTCP;

class TCPClient {
public:
    TCPClient(CommOfTCP *comm);
    ~TCPClient();

    /*!
     * @brief         通讯连接函数
     * 
     * @attention
     */
    void connect();

    /*!
     * @brief         通讯断开连接
     * 
     * @attention
     */
    void disconnect();

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
     * @brief         连接触发回调函数，用来判断是否连接成功
     * 
     * @param         ec
     * @attention
     */
    void on_connect(const boost::system::error_code& ec);

    /*!
     * @brief         定时器到时触发回调函数，用来处理到时之后的执行的操作
     * 
     * @param         ec
     * @attention
     */
    void on_timeout(const boost::system::error_code& ec);

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

private:
    CommOfTCP *m_comm;
    boost::asio::io_context* io_context_;
    boost::asio::ip::tcp::socket *socket_;
    boost::asio::deadline_timer *timer_;
    bool started_;
    bool startting_;
    std::thread io_context_run;
    std::atomic<bool> stop;
    std::vector<char> buffer_;
    std::vector<char> buf_;
};

class CommOfTCP : public CommunicationInterface
{
public:
    explicit CommOfTCP();
    ~CommOfTCP() override;

    /*!
     * @brief         连接接口
     * 
     * @attention
     */
    virtual void link() override;

    /*!
     * @brief         断开连接接口
     * 
     * @attention
     */
    virtual void unlink() override;

    /*!
     * @brief         发送消息接口
     * 
     * @param         buf
     * @attention
     */
    virtual void write_drive(std::vector<char> buf) override;

private:
    TCPClient *tcp;
};

#endif //TCPCLIENT_H
