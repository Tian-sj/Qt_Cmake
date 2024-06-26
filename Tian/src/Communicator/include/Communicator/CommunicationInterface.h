/*!
 * @file          CommunicationInterface.h
 * @brief         通讯接口类
 * @author        Tian_sj
 * @date          2024-03-04
 */
#ifndef TINA_COMMUNICATIONINTERFACE_H
#define TINA_COMMUNICATIONINTERFACE_H

#include <map>
#include <variant>
#include <string>
#include <optional>
#include <functional>
#include <vector>
#include <Communicator/MsgQueue.h>
#include <thread>
#include <mutex>
#include <Communicator/Communicator.h>

class CommunicationInterface
{
public:

    explicit CommunicationInterface();
    virtual ~CommunicationInterface();

    /*!
     * @brief         设置连接状态
     * 
     * @param         is_link
     * @attention
     */
    virtual void set_link(bool is_link);

    /*!
     * @brief         设置参数
     * 
     * @param         param_name
     * @param         param_value
     * @attention
     */
    virtual void set_parameter(const std::string &param_name, const std::variant<unsigned short, std::string> &param_value);

    /*!
     * @brief         获取参数
     * 
     * @param         param_name
     * @return        std::optional<std::variant<unsigned short, std::string> >
     * @attention
     */
    virtual std::optional<std::variant<unsigned short, std::string> > get_parameter(const std::string &param_name);

    /*!
     * @brief         连接接口
     * 
     * @attention
     */
    virtual void link() = 0;

    /*!
     * @brief         断开接口
     * 
     * @attention
     */
    virtual void unlink() = 0;

    /*!
     * @brief         发送消息
     * 
     * @param         buf
     * @attention
     */
    virtual void send(std::vector<char> buf);

    /*!
     * @brief         接收消息
     * 
     * @param         buffer
     * @attention
     */
    virtual void receive(std::vector<char> buffer);

    /*!
     * @brief         获取接收消息队列中的下一条消息
     * 
     * @return        Communicator::Reply
     * @attention
     */
    Communicator::Reply get_next_receiver();

    /*!
     * @brief         设置消息发送消息间隔
     * 
     * @param         delay_time
     */
    virtual void set_delay_time(int delay_time);

    /*!
     * @brief         以十六进制形式输出
     *
     * @param         buffer 传入的消息
     */
    void display_hex_buffer(std::vector<char> buffer);

protected:
    /*!
     * @brief         发送消息接口函数
     * 
     * @param         buf
     * @attention
     */
    virtual void write_drive(std::vector<char> buf) = 0;

    /*!
     * @brief         结束并退出线程
     * 
     * @attention
     */
    virtual void stop();

private:
    bool m_is_link;
    std::map<std::string, std::variant<unsigned short, std::string> > m_comm_params;
    MsgQueue<std::vector<char> > send_queue;
    MsgQueue<Communicator::Reply> receive_queue;

    std::thread* send_delay_thread;
    bool thread_close;
    int delay_time;

public:
    using emit_is_link = std::function<void(bool)>;
    /*!
     * @brief         注册函数，用于连接状态改变时候触发回调
     * 
     * @param         emit
     * @attention
     */
    virtual void register_is_link(emit_is_link emit);

private:
    emit_is_link emit_is_link_;
    std::mutex mtx_;
};

#endif // TIAN_COMMUNICATIONINTERFACE_H
