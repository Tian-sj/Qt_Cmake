#include <Communicator/SerialPortClient.h>
#include <iostream>

SerialPortClient::SerialPortClient(CommOfSerialPort *comm) 
        : m_comm(comm) 
        , started_(false)
        , stop(false)
        , io_context_(new boost::asio::io_context())
        , serial_port_(nullptr)
        , buffer_(BYTES_MAX_SIZE)
        , buf_(BYTES_MAX_SIZE)
{

    io_context_run = std::thread([&](){
        while(!stop) {
            io_context_->restart();
            io_context_->run();
        }
    });
}

SerialPortClient::~SerialPortClient()
{
    stop = true;
    close();
    io_context_run.join();
}

void SerialPortClient::open()
{
    if (serial_port_ != nullptr) {
        delete serial_port_;
        serial_port_ = nullptr;
    }

    boost::system::error_code ec;
    serial_port_->open(std::get<std::string>(m_comm->get_parameter("SerialPort_device").value()), ec);

    if (ec) {
        std::cout << "SerialPort Open error: " << ec.message() << std::endl;
        started_ = false;
        return;
    }

    serial_port_->set_option(boost::asio::serial_port::baud_rate(get_baud_rate()), ec);
    if (ec) {
        std::cout << "SerialPort Open error: " << ec.message() << std::endl;
        started_ = false;
        return;
    }

    serial_port_->set_option(boost::asio::serial_port::parity(get_parity()), ec);
    if (ec) {
        std::cout << "SerialPort Open error: " << ec.message() << std::endl;
        started_ = false;
        return;
    }

    serial_port_->set_option(boost::asio::serial_port::flow_control(get_flow_control()), ec);
    if (ec) {
        std::cout << "SerialPort Open error: " << ec.message() << std::endl;
        started_ = false;
        return;
    }

    serial_port_->set_option(boost::asio::serial_port::stop_bits(get_stop_bits()), ec);
    if (ec) {
        std::cout << "SerialPort Open error: " << ec.message() << std::endl;
        started_ = false;
        return;
    }

    serial_port_->set_option(boost::asio::serial_port::character_size(get_character_size()), ec);
    if (ec) {
        std::cout << "SerialPort Open error: " << ec.message() << std::endl;
        started_ = false;
        return;
    }

    std::cout << "SerialPort Opened!" << std::endl;
    started_ = true;
    m_comm->set_link(started_);
    readToSocket();
}

boost::asio::serial_port::baud_rate SerialPortClient::get_baud_rate()
{
    return boost::asio::serial_port::baud_rate(std::get<unsigned short>(m_comm->get_parameter("SerialPort_baud_rate").value()));
}

boost::asio::serial_port::parity SerialPortClient::get_parity()
{
    boost::asio::serial_port::parity::type type;
    switch (std::get<unsigned short>(m_comm->get_parameter("SerialPort_parity").value()))
    {
    case 0:
        type = boost::asio::serial_port::parity::none;
        break;
    case 1:
        type = boost::asio::serial_port::parity::odd;
        break;
    case 2:
        type = boost::asio::serial_port::parity::even;
        break;
    
    default:
        type = boost::asio::serial_port::parity::none;
        break;
    }
    return boost::asio::serial_port::parity(type);
}

boost::asio::serial_port::flow_control SerialPortClient::get_flow_control()
{
    boost::asio::serial_port::flow_control::type type;
    if (std::get<std::string>(m_comm->get_parameter("SerialPort_flow_control").value()) == "Software")
        type = boost::asio::serial_port::flow_control::software;
    else if (std::get<std::string>(m_comm->get_parameter("SerialPort_flow_control").value()) == "Hardware") 
        type = boost::asio::serial_port::flow_control::hardware;
    else 
        type = boost::asio::serial_port::flow_control::none;

    return boost::asio::serial_port::flow_control(type);
}

boost::asio::serial_port::stop_bits SerialPortClient::get_stop_bits()
{
    boost::asio::serial_port::stop_bits::type type;
    if (std::get<std::string>(m_comm->get_parameter("SerialPort_stop_bits").value()) == "1")
        type = boost::asio::serial_port::stop_bits::one;
    else if (std::get<std::string>(m_comm->get_parameter("SerialPort_stop_bits").value()) == "1.5")
        type = boost::asio::serial_port::stop_bits::onepointfive;
    else if (std::get<std::string>(m_comm->get_parameter("SerialPort_stop_bits").value()) == "2")
        type = boost::asio::serial_port::stop_bits::two;

    return boost::asio::serial_port::stop_bits(type);
}

boost::asio::serial_port::character_size SerialPortClient::get_character_size()
{
    return boost::asio::serial_port::character_size(std::get<unsigned short>(m_comm->get_parameter("SerialPort_character_size").value()));
}

void SerialPortClient::close()
{
    if (!serial_port_->is_open())
        return;

    std::cout << "disconnect" << std::endl;
    started_ = false;
    serial_port_->close();
    m_comm->set_link(started_);
}

void SerialPortClient::writeToSocket(const std::vector<char>& buf)
{
    if (!serial_port_)
        return;
    buf_ = buf;
    serial_port_->async_write_some(boost::asio::buffer(buf_), [this](auto && PH1, auto && PH2) {
        on_write(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    });
}

void SerialPortClient::on_write(const boost::system::error_code& ec, std::size_t bytes)
{
    if (!ec) {
        std::cout << "write successfully" << std::endl;
    } else {
        std::cout << "write error" << std::endl;
        if (started_)
            close();
    }
}

void SerialPortClient::readToSocket()
{
    if (!serial_port_)
        return;
    serial_port_->async_read_some(boost::asio::buffer(buffer_), [this](auto && PH1, auto && PH2) {
        on_read(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    });
}

void SerialPortClient::on_read(const boost::system::error_code& ec, std::size_t bytes)
{
    if (!ec) {
        std::cout << "read successfully" << bytes << std::endl;
        if (bytes != 0) {
            buffer_.resize(bytes);
            m_comm->receive(buffer_);
            buffer_.clear();
            buffer_.resize(BYTES_MAX_SIZE);
        }
        readToSocket();
    } else {
        std::cout << "read error" << std::endl;
        if (started_)
            close();
    }
}