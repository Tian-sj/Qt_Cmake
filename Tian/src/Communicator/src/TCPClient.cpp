#include <Communicator/TCPClient.h>
#include <iostream>

TCPClient::TCPClient(CommOfTCP *comm) :
        m_comm(comm),
        started_(false),
        startting_(false),
        stop(false),
        io_context_(new boost::asio::io_context()),
        socket_(nullptr),
        timer_(new boost::asio::deadline_timer(*io_context_)),
        buffer_(BYTES_MAX_SIZE),
        buf_(BYTES_MAX_SIZE)
{
    io_context_run = std::thread([&](){
        while(!stop) {
            io_context_->restart();
            io_context_->run();
        }
    });
}

TCPClient::~TCPClient()
{
    stop = true;
    disconnect();
    io_context_run.join();
}

void TCPClient::connect() {
    if (startting_)
        return;
    std::string ip = std::get<std::string>(m_comm->get_parameter("TCP_IP").value());
    unsigned short port = std::get<unsigned short>(m_comm->get_parameter("TCP_Port").value());
    boost::system::error_code ec;
    boost::asio::ip::address address = boost::asio::ip::address::from_string(ip, ec);
    boost::asio::ip::tcp::endpoint ep(address, port);

    if (socket_ != nullptr) {
        delete socket_;
        socket_ = nullptr;
    }

    socket_ = new boost::asio::ip::tcp::socket(*io_context_);
    timer_->expires_from_now(boost::posix_time::seconds(3));
    socket_->async_connect(ep, [this](auto && PH1) { on_connect(std::forward<decltype(PH1)>(PH1)); });
    startting_ = true;
    timer_->async_wait([this](auto && PH1) { on_timeout(std::forward<decltype(PH1)>(PH1)); });
}

void TCPClient::on_connect(const boost::system::error_code &ec) {
    // 取消截止时间定时器
    timer_->cancel();

    if (!ec) {
        std::cout << "Connected to the server!" << std::endl;
        started_ = true;
        readToSocket();
    } else {
        std::cerr << "Connection error: " << ec.message() << std::endl;
        started_ = false;
    }
    startting_ = false;
    m_comm->set_link(started_);
}

void TCPClient::on_timeout(const boost::system::error_code &ec) {
    if (ec == boost::asio::error::operation_aborted) {
        return;
    }

    std::cerr << "Connection timeout!" << std::endl;
    disconnect();
}

void TCPClient::disconnect() {
    if (!socket_->is_open())
        return;
    std::cout << "disconnect" << std::endl;
    startting_ = false;
    started_ = false;
    socket_->close();
    m_comm->set_link(started_);
}

void TCPClient::writeToSocket(const std::vector<char> &buf) {
    if (!socket_)
        return;
    buf_ = buf;
    std::cout << "send: ";
    m_comm->display_hex_buffer(buf);
    socket_->async_send(boost::asio::buffer(buf_), [this](auto && PH1, auto && PH2) {
        on_write(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    });
}

void TCPClient::on_write(const boost::system::error_code &ec, std::size_t bytes) {
    if (!ec) {
        // std::cout << "send successfully" << std::endl;
    } else {
        // std::cout << "send error" << std::endl;
        if (started_)
            disconnect();
    }
}

void TCPClient::readToSocket() {
    if (!socket_)
        return;
    socket_->async_receive(boost::asio::buffer(buffer_), [this](auto && PH1, auto && PH2) {
        on_read(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    });
}

void TCPClient::on_read(const boost::system::error_code &ec, std::size_t bytes) {
    if (!ec) {
        // std::cout << "read successfully" << bytes << std::endl;
        if (bytes != 0) {
            buffer_.resize(bytes);
            m_comm->receive(buffer_);
            std::cout << "read: ";
            m_comm->display_hex_buffer(buffer_);
            buffer_.clear();
            buffer_.resize(BYTES_MAX_SIZE);
        }
        readToSocket();
    } else {
        // std::cout << "read error" << std::endl;
        if (started_)
            disconnect();
    }
}

CommOfTCP::CommOfTCP() : CommunicationInterface()
{
    tcp = new TCPClient(this);
}

CommOfTCP::~CommOfTCP()
{
    delete tcp;
    tcp = nullptr;
}

void CommOfTCP::link()
{
    tcp->connect();
}

void CommOfTCP::unlink()
{
    tcp->disconnect();
}

void CommOfTCP::write_drive(std::vector<char> buf)
{
    if (!buf.empty())
        tcp->writeToSocket(buf);
}
