#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <boost/asio.hpp>
#include <thread>
#include <Communicator/CommunicationInterface.h>

#define BYTES_MAX_SIZE 1024

class CommOfUDP;

class UDPClient {
public:
    UDPClient(CommOfUDP *comm);
    ~UDPClient();

    /*!
     * @brief         打开通讯连接
     * 
     * @attention
     */
    void open();

    /*!
     * @brief         关闭通讯连接
     * 
     * @attention
     */
    void close();

    /*!
     * @brief         消息发送
     * 
     * @param         buf
     * @attention
     */
    void writeToSocket(const std::vector<char>& buf);

private:
    /*!
     * @brief         消息发送触发回调函数，用来判断是否发送成功
     * 
     * @param         ec
     * @param         bytes
     * @attention
     */
    void on_write(const boost::system::error_code& ec, std::size_t bytes);

private:
    CommOfUDP *m_comm;
    boost::asio::io_context* io_context_;
    boost::asio::ip::udp::socket *socket_;
    boost::asio::ip::udp::endpoint ep;
    std::thread io_context_run;
    std::atomic<bool> stop;
    std::vector<char> buffer_;
};

class CommOfUDP : public CommunicationInterface
{
public:
    explicit CommOfUDP();
    ~CommOfUDP() override;

    virtual void link() override;
    virtual void unlink() override;

    virtual void write_drive(std::vector<char> buf) override;

private:
    UDPClient *udp;
};

#endif //UDPCLIENT_H
