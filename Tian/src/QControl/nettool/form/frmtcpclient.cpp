﻿#include "frmtcpclient.h"
#include "ui_frmtcpclient.h"
#include "qthelper.h"
#include "qthelperdata.h"

frmTcpClient::frmTcpClient(QWidget *parent) : QWidget(parent), ui(new Ui::frmTcpClient)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmTcpClient::~frmTcpClient()
{
    delete ui;
}

void frmTcpClient::initUIText()
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
    ui->btnConnect->setText(tr("Connect"));  // 连接
    ui->btnSave->setText(tr("Save"));  // 保存
    ui->btnClear->setText(tr("Clear"));  // 清空
    ui->btnSend->setText(tr("Send"));  // 发送
}

bool frmTcpClient::eventFilter(QObject *watched, QEvent *event)
{
    //双击清空
    if (watched == ui->txtMain->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            on_btnClear_clicked();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmTcpClient::initForm()
{
    QFont font;
    font.setPixelSize(16);
    ui->txtMain->setFont(font);
    ui->txtMain->viewport()->installEventFilter(this);

    isOk = false;

    //实例化对象并绑定信号槽
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
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
    QtHelper::initLocalIPs(ui->cboxBindIP, AppConfig::TcpBindIP);
}

void frmTcpClient::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendTcpClient);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveTcpClient);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiTcpClient);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugTcpClient);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendTcpClient);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalTcpClient)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxBindIP->setCurrentIndex(ui->cboxBindIP->findText(AppConfig::TcpBindIP));
    connect(ui->cboxBindIP, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtBindPort->setText(QString::number(AppConfig::TcpBindPort));
    connect(ui->txtBindPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(AppConfig::TcpServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(AppConfig::TcpServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    this->initTimer();
}

void frmTcpClient::saveConfig()
{
    AppConfig::HexSendTcpClient = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveTcpClient = ui->ckHexReceive->isChecked();
    AppConfig::AsciiTcpClient = ui->ckAscii->isChecked();
    AppConfig::DebugTcpClient = ui->ckDebug->isChecked();
    AppConfig::AutoSendTcpClient = ui->ckAutoSend->isChecked();
    AppConfig::IntervalTcpClient = ui->cboxInterval->currentText().toInt();
    AppConfig::TcpBindIP = ui->cboxBindIP->currentText();
    AppConfig::TcpBindPort = ui->txtBindPort->text().trimmed().toInt();
    AppConfig::TcpServerIP = ui->txtServerIP->text().trimmed();
    AppConfig::TcpServerPort = ui->txtServerPort->text().trimmed().toInt();
    AppConfig::writeConfig();

    this->initTimer();
}

void frmTcpClient::initTimer()
{
    if (timer->interval() != AppConfig::IntervalTcpClient) {
        timer->setInterval(AppConfig::IntervalTcpClient);
    }

    if (AppConfig::AutoSendTcpClient) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void frmTcpClient::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;
    QtHelper::appendMsg(ui->txtMain, type, data, maxCount, currentCount, clear, ui->ckShow->isChecked());
}

void frmTcpClient::connected()
{
    isOk = true;
    ui->btnConnect->setText(tr("Disconnect"));
    append(2, tr("Server Connection"));
    append(4, QString(tr("Local Address: %1  Local Port: %2")).arg(socket->localAddress().toString()).arg(socket->localPort()));
    append(4, QString(tr("Remote Address: %1  Remote Port: %2")).arg(socket->peerAddress().toString()).arg(socket->peerPort()));
}

void frmTcpClient::disconnected()
{
    isOk = false;
    ui->btnConnect->setText(tr("Connect"));
    append(2, tr("Server Disconnected"));
}

void frmTcpClient::error()
{
    append(3, socket->errorString());
}

void frmTcpClient::readData()
{
    QByteArray data = socket->readAll();
    if (data.length() <= 0) {
        return;
    }

    QString buffer;
    if (AppConfig::HexReceiveTcpClient) {
        buffer = QtHelperData::byteArrayToHexStr(data);
    } else if (AppConfig::AsciiTcpClient) {
        buffer = QtHelperData::byteArrayToAsciiStr(data);
    } else {
        buffer = QString(data);
    }

    append(1, buffer);

    //自动回复数据,可以回复的数据是以;隔开,每行可以带多个;所以这里不需要继续判断
    if (AppConfig::DebugTcpClient) {
        int count = AppData::Keys.count();
        for (int i = 0; i < count; i++) {
            if (AppData::Keys.at(i) == buffer) {
                sendData(AppData::Values.at(i));
                break;
            }
        }
    }
}

void frmTcpClient::sendData(const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendTcpClient) {
        buffer = QtHelperData::hexStrToByteArray(data);
    } else if (AppConfig::AsciiTcpClient) {
        buffer = QtHelperData::asciiStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    socket->write(buffer);
    append(0, data);
}

void frmTcpClient::on_btnConnect_clicked()
{
    if (ui->btnConnect->text() == "Connect") {
        //断开所有连接和操作
        //socket->abort();
        //绑定网卡和端口
        //有个后遗症,客户端这边断开连接后还会保持几分钟导致不能重复绑定
        //如果是服务器断开则可以继续使用
        //提示 The bound address is already in use
        //参考 https://www.cnblogs.com/baiduboy/p/7426822.html
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        //socket->bind(QHostAddress(AppConfig::TcpBindIP), AppConfig::TcpBindPort);
#endif
        //连接服务器
        socket->connectToHost(AppConfig::TcpServerIP, AppConfig::TcpServerPort);
    } else {
        socket->abort();
    }
}

void frmTcpClient::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppData::saveData(data);
    on_btnClear_clicked();
}

void frmTcpClient::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmTcpClient::on_btnSend_clicked()
{
    if (!isOk) {
        return;
    }

    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }

    sendData(data);
}
