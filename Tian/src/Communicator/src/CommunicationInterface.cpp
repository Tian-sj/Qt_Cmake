#include <Communicator/CommunicationInterface.h>
#include <iostream>
#include <iomanip>

CommunicationInterface::CommunicationInterface() :
    m_is_link(false),
    thread_close(false),
    send_queue(100),
    receive_queue(100),
    delay_time(50)
{
    send_delay_thread = new std::thread([this](){
        while (!thread_close) {
            {
            if (thread_close)
                break;
            }
            if (!m_is_link)
                continue;

            Communicator::Reply reply;
            reply.sender = send_queue.pop();
            receive_queue.push(reply);
            write_drive(reply.sender);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_time));
        }
    });
}

CommunicationInterface::~CommunicationInterface() {

}

void CommunicationInterface::set_link(bool is_link)
{
    if (is_link != m_is_link)
    {
        send_queue.clear();
        receive_queue.clear();

        m_is_link = is_link;

        if (emit_is_link_)
            emit_is_link_(m_is_link);
    }
}

void CommunicationInterface::set_parameter(const std::string &param_name, const std::variant<unsigned short, std::string> &param_value) {
    this->m_comm_params[param_name] = param_value;
}

std::optional<std::variant<unsigned short, std::string>> CommunicationInterface::get_parameter(const std::string &param_name) {
    auto it = m_comm_params.find(param_name);
    if (it != m_comm_params.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}

void CommunicationInterface::send(std::vector<char> buf)
{
    send_queue.push(buf);
}

void CommunicationInterface::receive(std::vector<char> buffer)
{
    try {
        Communicator::Reply reply;
        receive_queue.back(reply);
        reply.result = buffer;
        receive_queue.update_back(reply);
    } catch (...) {
        std::cout << "receive is empty!" << std::endl;
    }
}

Communicator::Reply CommunicationInterface::get_next_receiver()
{
    if (receive_queue.empty())
        return Communicator::Reply();

    if (!receive_queue.front().result.empty()) {
        return receive_queue.pop();
    }

    if (receive_queue.size() > 1) {
        receive_queue.try_pop();
        return get_next_receiver();
    }

    return Communicator::Reply();
}

void CommunicationInterface::stop()
{
    {
        std::unique_lock<std::mutex> lock(mtx_);
        thread_close = true;
    }
    send_delay_thread->join();

    if (send_delay_thread) {
        delete send_delay_thread;
        send_delay_thread = nullptr;
    }
}

void CommunicationInterface::register_is_link(emit_is_link emit)
{
    emit_is_link_ = emit;
}

void CommunicationInterface::set_delay_time(int delay_time) {
    this->delay_time = delay_time;
}

void CommunicationInterface::display_hex_buffer(std::vector<char> buffer)
{
    for (auto &c : buffer) {
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                  << static_cast<int>(static_cast<uint8_t>(c)) << " ";
    }

    std::cout << std::endl;
}
