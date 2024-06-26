/*!
 * @file          Communicator.h
 * @brief         对外通讯接口类
 * @author        Tian_sj
 * @date          2024-03-04
 */
#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <string>
#include <variant>
#include <vector>
#include <functional>
#include <optional>

class CommunicationInterface;

class Communicator
{
public:
    /*!
     * @brief         存放发送的消息，和接收到的消息
     */
    struct Reply {
        std::vector<char> sender;
        std::vector<char> result;
    };

    /*!
     * @brief         通讯接口类型
     */
    enum class INTERFACE{
        TCP = 0,
        SERIAL_PORT,
        UDP,
        NULL_ = 999
    };

    explicit Communicator();
    ~Communicator();

    /*!
     * @brief         切换指定的通讯接口
     * 
     * @param         interface
     * @attention
     */
    void toggle_interface(INTERFACE interface);

    /*!
     * @brief         连接
     * 
     * @attention
     */
    void link();

    /*!
     * @brief         断开
     * 
     * @attention
     */
    void unlink();

    /*!
     * @brief         设置需要的参数
     * 
     * @param         param_name
     * @param         param_value
     * @attention
     */
    void set_parameter(const std::string &param_name, const std::variant<unsigned short, std::string> &param_value);

    /*!
     * @brief         获取参数
     * 
     * @param         param_name
     * @return        std::optional<std::variant<unsigned short, std::string> >
     * @attention
     */
    virtual std::optional<std::variant<unsigned short, std::string> > get_parameter(const std::string &param_name);

    /*!
     * @brief         设置发送消息间隔
     * 
     * @param         delay_time
     * @attention
     */
    void set_delay_time(int delay_time);;

    /*!
     * @brief         发送消息
     * 
     * @param         buf
     * @attention
     */
    void send(std::vector<char> buf);

    /*!
     * @brief         获取下一个接收消息队列中的消息
     * 
     * @return        Reply
     * @attention
     */
    virtual Reply get_next_receiver();

public:
    /*!
     * @brief         注册连接状态函数
     * 
     * @param         link
     * @attention
     */
    void register_link(std::function<void(bool)> link);

private:
    /*!
     * @brief         按照当前通讯接口初始化
     * 
     * @attention
     */
    void set_interface();

private:
    CommunicationInterface *comm;
    std::function<void(bool)> is_link;

    INTERFACE m_interface;

};

#endif // COMMUNICATOR_H
