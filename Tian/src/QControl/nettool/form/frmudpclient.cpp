﻿#include "frmudpclient.h"
#include "ui_frmudpclient.h"
#include "qthelper.h"
#include "qthelperdata.h"

frmUdpClient::frmUdpClient(QWidget *parent) : QWidget(parent), ui(new Ui::frmUdpClient)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmUdpClient::~frmUdpClient()
{
    delete ui;
}

void frmUdpClient::initUIText()
{
    ui->ckHexReceive->setText(tr("Hex Receive"));  // 16进制接收
    ui->ckHexSend->setText(tr("Hex Send"));  // 16进制发送
    ui->ckAscii->setText(tr("Control Chars"));  // 控制字符
    ui->ckShow->setText(tr("Pause Display"));  // 暂停显示
    ui->ckDebug->setText(tr("Simulate Device"));  // 模拟设备
    ui->ckAutoSend->setText(tr("Auto Send"));  // 定时发送
    ui->labBindIP->setText(tr("Bind IP"));  // 绑定地址
    ui->labBindPort->setText(tr("Bind Port"));  // 绑定端口
    ui->labServerIP->setText(tr("Server IP"));  // 服务器地址
    ui->labServerPort->setText(tr("Server Port"));  // 服务器端口
    ui->btnSave->setText(tr("Save"));  // 保存
    ui->btnClear->setText(tr("Clear"));  // 清空
    ui->btnSend->setText(tr("Send"));  // 发送
}

bool frmUdpClient::eventFilter(QObject *watched, QEvent *event)
{
    //双击清空
    if (watched == ui->txtMain->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            on_btnClear_clicked();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmUdpClient::initForm()
{
    QFont font;
    font.setPixelSize(16);
    ui->txtMain->setFont(font);
    ui->txtMain->viewport()->installEventFilter(this);

    //实例化对象并绑定信号槽
    socket = new QUdpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(error()));
#else
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
#endif    

    //定时器发送数据
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    //填充数据到下拉框
    ui->cboxInterval->addItems(AppData::Intervals);
    ui->cboxData->addItems(AppData::Datas);
    QtHelper::initLocalIPs(ui->cboxBindIP, AppConfig::UdpBindIP);
}

void frmUdpClient::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendUdpClient);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveUdpClient);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiUdpClient);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugUdpClient);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendUdpClient);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalUdpClient)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxBindIP->setCurrentIndex(ui->cboxBindIP->findText(AppConfig::UdpBindIP));
    connect(ui->cboxBindIP, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtBindPort->setText(QString::number(AppConfig::UdpBindPort));
    connect(ui->txtBindPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(AppConfig::UdpServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(AppConfig::UdpServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    this->initTimer();
}

void frmUdpClient::saveConfig()
{
    AppConfig::HexSendUdpClient = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveUdpClient = ui->ckHexReceive->isChecked();
    AppConfig::AsciiUdpClient = ui->ckAscii->isChecked();
    AppConfig::DebugUdpClient = ui->ckDebug->isChecked();
    AppConfig::AutoSendUdpClient = ui->ckAutoSend->isChecked();
    AppConfig::IntervalUdpClient = ui->cboxInterval->currentText().toInt();
    AppConfig::UdpBindIP = ui->cboxBindIP->currentText();
    AppConfig::UdpBindPort = ui->txtBindPort->text().trimmed().toInt();
    AppConfig::UdpServerIP = ui->txtServerIP->text().trimmed();
    AppConfig::UdpServerPort = ui->txtServerPort->text().trimmed().toInt();
    AppConfig::writeConfig();

    this->initTimer();
}

void frmUdpClient::initTimer()
{
    if (timer->interval() != AppConfig::IntervalUdpClient) {
        timer->setInterval(AppConfig::IntervalUdpClient);
    }

    if (AppConfig::AutoSendUdpClient) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void frmUdpClient::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;
    QtHelper::appendMsg(ui->txtMain, type, data, maxCount, currentCount, clear, ui->ckShow->isChecked());
}

void frmUdpClient::error()
{
    append(3, socket->errorString());
}

void frmUdpClient::readData()
{
    QHostAddress host;
    quint16 port;
    QByteArray data;
    QString buffer;

    while (socket->hasPendingDatagrams()) {
        data.resize(socket->pendingDatagramSize());
        socket->readDatagram(data.data(), data.size(), &host, &port);

        if (AppConfig::HexReceiveUdpClient) {
            buffer = QtHelperData::byteArrayToHexStr(data);
        } else if (AppConfig::AsciiUdpClient) {
            buffer = QtHelperData::byteArrayToAsciiStr(data);
        } else {
            buffer = QString(data);
        }

        QString ip = host.toString();
        ip = ip.replace("::ffff:", "");
        if (ip.isEmpty()) {
            continue;
        }

        QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(buffer);
        append(1, str);

        if (AppConfig::DebugUdpClient) {
            int count = AppData::Keys.count();
            for (int i = 0; i < count; i++) {
                if (AppData::Keys.at(i) == buffer) {
                    sendData(ip, port, AppData::Values.at(i));
                    break;
                }
            }
        }
    }
}

void frmUdpClient::sendData(const QString &ip, int port, const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendUdpClient) {
        buffer = QtHelperData::hexStrToByteArray(data);
    } else if (AppConfig::AsciiUdpClient) {
        buffer = QtHelperData::asciiStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    //绑定网卡和端口,没有绑定过才需要绑定
    //采用端口是否一样来判断是为了方便可以直接动态绑定切换端口
    if (socket->localPort() != AppConfig::UdpBindPort) {
        socket->abort();
        socket->bind(QHostAddress(AppConfig::UdpBindIP), AppConfig::UdpBindPort);
    }

    //指定地址和端口发送数据
    socket->writeDatagram(buffer, QHostAddress(ip), port);

    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    append(0, str);
}

void frmUdpClient::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppData::saveData(data);
    on_btnClear_clicked();
}

void frmUdpClient::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmUdpClient::on_btnSend_clicked()
{
    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }

    sendData(AppConfig::UdpServerIP, AppConfig::UdpServerPort, data);
}
