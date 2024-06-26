/*!
 * @file          CommProtocols.h
 * @brief         通讯协议通用类
 * @author        Tian_sj
 * @date          2024-03-04
 */
#ifndef TIAN_COMMPROTOCOLS_H
#define TIAN_COMMPROTOCOLS_H

#include <functional>
#include <vector>
#include <thread>
#include <variant>

class Communicator;

namespace Tian {
    class CommProtocols {
    public:
        explicit CommProtocols();

        virtual ~CommProtocols();

        /*!
         * @brief         通讯接口类型
         * 
         * @attention
         */
        enum class COMM_INTERFACE {
            TCP = 0,
            SERIAL_PORTI,
            UDP
        };

    public:
        /*!
         * @brief         设置相应的通讯接口的通讯参数
         * 
         * @param         param_name
         * @param         param_value
         * @attention
         */
        void set_parameter(const std::string &param_name, const std::variant<unsigned short, std::string> &param_value);

        /*!
         * @brief         设置对外接口UDP的通讯参数
         * 
         * @param         param_name
         * @param         param_value
         * @attention
         */
        void set_UDP_parameter(const std::string &param_name, const std::variant<unsigned short, std::string> &param_value);

        /*!
         * @brief         设置指定的通讯接口
         * 
         * @param         comm_interface
         * @attention
         */
        virtual void set_Comm_Interface(COMM_INTERFACE comm_interface);

        /*!
         * @brief         设置对外接口的通讯接口为UDP
         * 
         * @param         comm_interface
         * @attention
         */
        virtual void set_UDP_Interface(COMM_INTERFACE comm_interface);

        /*!
         * @brief         连接
         * 
         * @attention
         */
        virtual void link();

        /*!
         * @brief         断开连接
         * 
         * @attention
         */
        virtual void unlink();

        /*!
         * @brief         打开对外接口
         * 
         * @attention
         */
        virtual void open_udp();

        /*!
         * @brief         关闭对外接口
         * 
         * @attention
         */
        virtual void close_udp();

    public:
        using emit_link = std::function<void(bool)>;

        /*!
         * @brief         注册函数，状态改变时候触发回调
         * 
         * @param         link
         * @attention
         */
        void register_is_link(emit_link link);

    protected:
        /*!
         * @brief         开始解析指令
         */
        virtual void analyze() = 0;


        /*!
         * @brief         创建通信帧
         *
         * @return        std::vector<char> 通信帧
         */
        virtual std::vector<char> createCommunicator() = 0;

    protected:
        Communicator *comm;
        Communicator *udp;
        bool is_link;
        bool analyze_thread_running;
        bool analyze_exit;

    private:
        std::vector<emit_link> emit_link_vector;

        std::thread analyze_thread;
    };
}

#endif // TIAN_COMMPROTOCOLS_H
