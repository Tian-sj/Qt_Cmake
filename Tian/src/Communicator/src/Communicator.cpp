#include <Communicator/Communicator.h>
#include <Communicator/TCPClient.h>
#include <Communicator/UDPClient.h>
#include <iostream>

Communicator::Communicator() :
    m_interface(INTERFACE::NULL_),
    comm(nullptr)
{
}

Communicator::~Communicator()
{
    if (comm != nullptr) {
        delete comm;
        comm = nullptr;
    }
}

void Communicator::toggle_interface(INTERFACE interface) {
    if (interface == m_interface)
        return;

    m_interface = interface;
    if (comm) {
        delete comm;
        comm = nullptr;
    }
    set_interface();
}

void Communicator::link()
{
    comm->link();
}

void Communicator::unlink()
{
    comm->unlink();
}

void Communicator::set_parameter(const std::string &param_name, const std::variant<unsigned short, std::string> &param_value)
{
    comm->set_parameter(param_name, param_value);
}

std::optional<std::variant<unsigned short, std::string> > Communicator::get_parameter(const std::string &param_name)
{
    return comm->get_parameter(param_name);
}

void Communicator::send(std::vector<char> buf)
{
    comm->send(buf);
}

Communicator::Reply Communicator::get_next_receiver()
{
    return comm->get_next_receiver();
}

void Communicator::register_link(std::function<void (bool)> link)
{
    is_link = link;
}

void Communicator::set_interface()
{
    switch (m_interface) {
    case INTERFACE::TCP:
        comm = new CommOfTCP();
        comm->register_is_link(is_link);
        break;
    case INTERFACE::SERIAL_PORT:

        break;
    case INTERFACE::UDP:
        comm = new CommOfUDP();
        break;
    default:
        break;
    }
}

void Communicator::set_delay_time(int delay_time) {
    comm->set_delay_time(delay_time);
}
