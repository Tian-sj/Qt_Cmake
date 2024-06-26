#include <Communicator/UDPClient.h>
#include <iostream>

UDPClient::UDPClient(CommOfUDP *comm) :
        m_comm(comm),
        stop(false),
        io_context_(new boost::asio::io_context()),
        socket_(nullptr),
        buffer_(BYTES_MAX_SIZE)
{
    m_comm->set_link(true);
    io_context_run = std::thread([&](){
        while(!stop) {
            io_context_->restart();
            io_context_->run();
        }
    });
}

UDPClient::~UDPClient()
{
    stop = true;
    io_context_run.join();
    delete socket_;
    socket_ = nullptr;
    delete io_context_;
    io_context_ = nullptr;
}

void UDPClient::open() 
{
    if (socket_ != nullptr) {
        delete socket_;
        socket_ = nullptr;
    }

    std::string ip = std::get<std::string>(m_comm->get_parameter("UDP_IP").value());
    unsigned short port = std::get<unsigned short>(m_comm->get_parameter("UDP_Port").value());
    boost::system::error_code ec;
    boost::asio::ip::address address = boost::asio::ip::address::from_string(ip, ec);
    ep = boost::asio::ip::udp::endpoint(address, port);

    socket_ = new boost::asio::ip::udp::socket(*io_context_);
    socket_->open(boost::asio::ip::udp::v4());
}

void UDPClient::close()
{
    socket_->close();
}

void UDPClient::writeToSocket(const std::vector<char> &buf) {
    if (!socket_)
        return;
    buffer_ = buf;
    socket_->async_send_to(boost::asio::buffer(buffer_), ep, [this](auto && PH1, auto && PH2) {
        on_write(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    });
}

void UDPClient::on_write(const boost::system::error_code &ec, std::size_t bytes) {
    if (!ec) {
        std::cout << "write successfully" << std::endl;
    } else {
        std::cout << "write error: " << ec << std::endl;
    }
}

CommOfUDP::CommOfUDP() : CommunicationInterface()
{
    udp = new UDPClient(this);
}

CommOfUDP::~CommOfUDP()
{
    delete udp;
    udp = nullptr;
}

void CommOfUDP::link()
{
    udp->open();
}

void CommOfUDP::unlink()
{
    udp->close();
}

void CommOfUDP::write_drive(std::vector<char> buf)
{
    if (!buf.empty())
        udp->writeToSocket(buf);
}
