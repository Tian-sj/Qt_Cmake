#include <comm/tcpsocket.h>

TcpSocket::TcpSocket(CommOfEthernet *comm)
{
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    m_comm = comm;
    m_tcp = nullptr;
}

TcpSocket::~TcpSocket()
{
}

bool TcpSocket::connect_to_server()
{
    QVariant server_address, server_port;
    if (!m_comm->get_paramter("ServerAddress", server_address) ||
        !m_comm->get_paramter("ServerPort", server_port))
    {
        return false;
    }

    disconnect_from_server();

    m_tcp = new QTcpSocket(this);

    connect(m_tcp, &QTcpSocket::readyRead, this, &TcpSocket::handle_data);
    connect(m_tcp, &QTcpSocket::stateChanged, m_comm, &CommOfEthernet::onStateChanged);

    QString serverAddress = server_address.toString();
    int serverPort = server_port.toInt();
    m_tcp->connectToHost(serverAddress, serverPort);

    return true;
}

void TcpSocket::disconnect_from_server()
{
    if (m_tcp != nullptr)
    {
        disconnect(m_tcp, nullptr, nullptr, nullptr);
        m_tcp->close();
        delete m_tcp;
        m_tcp = nullptr;
    }
}

void TcpSocket::on_start()
{
    if (!connect_to_server())
        return;
}
void TcpSocket::on_stop()
{
    m_tcp->disconnect();
}

void TcpSocket::handle_data()
{
    QByteArray receive = m_tcp->readAll();

    m_comm->onReceived(receive);
}

void TcpSocket::write_data(const QByteArray data)
{
    if (m_tcp)
    {
        m_tcp->write(data);
        m_tcp->flush();
    }
}

CommOfEthernet::CommOfEthernet() : CommInterface()
{
    m_worker = new TcpSocket(this);
    m_thread = new QThread(this);
    m_worker->moveToThread(m_thread);
    connect(this, &CommOfEthernet::start_running, m_worker, &TcpSocket::on_start);
    connect(this, &CommOfEthernet::write_data, m_worker, &TcpSocket::write_data);
}

CommOfEthernet::~CommOfEthernet()
{
    if (is_open())
    {
        close();
    }
    m_worker->deleteLater();
}

bool CommOfEthernet::open()
{
    m_thread->start();
    emit start_running();

    return true;
}

bool CommOfEthernet::close()
{
    m_worker->on_stop();
    m_thread->quit();
    m_thread->wait();

    set_open(false);

    return true;
}

int CommOfEthernet::send_data(QByteArray order)
{
    emit write_data(order);
    return order.size();
}

void CommOfEthernet::onStateChanged(QAbstractSocket::SocketState state)
{
    switch (state)
    {
    case QAbstractSocket::ConnectedState:
    {
        set_open(true);
        break;
    }
    case QAbstractSocket::UnconnectedState:
        close();
        break;
    case QAbstractSocket::ListeningState:
    case QAbstractSocket::BoundState:
    case QAbstractSocket::ClosingState:
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::HostLookupState:
        break;
    }
}
