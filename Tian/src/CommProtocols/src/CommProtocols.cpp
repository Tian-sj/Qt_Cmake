#include <CommProtocols/CommProtocols.h>
#include <Communicator/Communicator.h>

namespace Tian {
    CommProtocols::CommProtocols() : analyze_exit(false),
    comm(nullptr),
    udp(new Communicator())
    {
        udp->toggle_interface(static_cast<Communicator::INTERFACE>(COMM_INTERFACE::UDP));
        is_link = false;
        comm = new Communicator();
        comm->register_link([this](bool link) {
            is_link = link;

            analyze_thread_running = link;

            for (auto it = emit_link_vector.begin(); it != emit_link_vector.end(); ++it)
                if (*it) {
                    (*it)(link);
                }
        });

        analyze_thread_running = false;
        analyze_thread = std::thread(&CommProtocols::analyze, this);
    }

    CommProtocols::~CommProtocols() {
        analyze_thread_running = false;
        analyze_exit = true;
        if (analyze_thread.joinable())
            analyze_thread.join();

        if (comm != nullptr) {
            delete comm;
            comm = nullptr;
        }

        if (udp != nullptr) {
            delete udp;
            udp = nullptr;
        }
    }

    void CommProtocols::set_Comm_Interface(COMM_INTERFACE comm_interface) {
        comm->toggle_interface(static_cast<Communicator::INTERFACE>(comm_interface));
    }

    void CommProtocols::set_UDP_Interface(COMM_INTERFACE comm_interface)
    {
        udp->toggle_interface(static_cast<Communicator::INTERFACE>(comm_interface));
    }

    void CommProtocols::set_parameter(const std::string &param_name, const std::variant<unsigned short, std::string> &param_value) {
        comm->set_parameter(param_name, param_value);
    }

    void CommProtocols::set_UDP_parameter(const std::string &param_name, const std::variant<unsigned short, std::string> &param_value)
    {
        udp->set_parameter(param_name, param_value);
    }

    void CommProtocols::link() {
        comm->link();
    }

    void CommProtocols::unlink() {
        comm->unlink();
    }

    void CommProtocols::open_udp()
    {
        udp->link();
    }

    void CommProtocols::close_udp()
    {
        udp->unlink();
    }

    void CommProtocols::register_is_link(emit_link link) {
        emit_link_vector.emplace_back(link);
        if (link)
            link(is_link);
    }
}
