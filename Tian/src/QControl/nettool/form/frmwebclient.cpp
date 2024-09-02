﻿#include "frmwebclient.h"
#include "ui_frmwebclient.h"
#include "qthelper.h"
#include "qthelperdata.h"

frmWebClient::frmWebClient(QWidget *parent) : QWidget(parent), ui(new Ui::frmWebClient)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmWebClient::~frmWebClient()
{
    delete ui;
}

void frmWebClient::initUIText()
{
    ui->ckHexReceive->setText(tr("Hex Receive"));  // 16进制接收
    ui->ckHexSend->setText(tr("Hex Send"));  // 16进制发送
    ui->ckAscii->setText(tr("Control Chars"));  // 控制字符
    ui->ckShow->setText(tr("Pause Display"));  // 暂停显示
    ui->ckDebug->setText(tr("Simulate Device"));  // 模拟设备
    ui->ckAutoSend->setText(tr("Auto Send"));  // 定时发送
    ui->labServerIP->setText(tr("Server IP"));  // 服务器地址
    ui->labServerPort->setText(tr("Server Port"));  // 服务器端口
    ui->btnConnect->setText(tr("Connect"));  // 连接
    ui->btnSave->setText(tr("Save"));  // 保存
    ui->btnClear->setText(tr("Clear"));  // 清空
    ui->btnSend->setText(tr("Send"));  // 发送
}

bool frmWebClient::eventFilter(QObject *watched, QEvent *event)
{
    //双击清空
    if (watched == ui->txtMain->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            on_btnClear_clicked();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmWebClient::initForm()
{
    QFont font;
    font.setPixelSize(16);
    ui->txtMain->setFont(font);
    ui->txtMain->viewport()->installEventFilter(this);

    isOk = false;

    //实例化对象并绑定信号槽
    socket = new QWebSocket("WebSocket", QWebSocketProtocol::VersionLatest, this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));

    //暂时使用前面两个信号,部分系统上后面两个信号Qt没实现,目前测试到5.15.2
    //在win上如果两组信号都关联了则都会触发,另外一组信号就是多个参数表示是否是最后一个数据包
    connect(socket, SIGNAL(textMessageReceived(QString)), this, SLOT(textMessageReceived(QString)));
    connect(socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(binaryMessageReceived(QByteArray)));
    //connect(socket, SIGNAL(textFrameReceived(QString, bool)), this, SLOT(textFrameReceived(QString, bool)));
    //connect(socket, SIGNAL(binaryFrameReceived(QByteArray, bool)), this, SLOT(binaryFrameReceived(QByteArray, bool)));

    //定时器发送数据
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    //填充数据到下拉框
    ui->cboxInterval->addItems(AppData::Intervals);
    ui->cboxData->addItems(AppData::Datas);
}

void frmWebClient::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendWebClient);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveWebClient);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiWebClient);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugWebClient);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendWebClient);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalWebClient)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(AppConfig::WebServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(AppConfig::WebServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    this->initTimer();
}

void frmWebClient::saveConfig()
{
    AppConfig::HexSendWebClient = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveWebClient = ui->ckHexReceive->isChecked();
    AppConfig::AsciiWebClient = ui->ckAscii->isChecked();
    AppConfig::DebugWebClient = ui->ckDebug->isChecked();
    AppConfig::AutoSendWebClient = ui->ckAutoSend->isChecked();
    AppConfig::IntervalWebClient = ui->cboxInterval->currentText().toInt();
    AppConfig::WebServerIP = ui->txtServerIP->text().trimmed();
    AppConfig::WebServerPort = ui->txtServerPort->text().trimmed().toInt();
    AppConfig::writeConfig();

    this->initTimer();
}

void frmWebClient::initTimer()
{
    if (timer->interval() != AppConfig::IntervalWebClient) {
        timer->setInterval(AppConfig::IntervalWebClient);
    }

    if (AppConfig::AutoSendWebClient) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void frmWebClient::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;
    QtHelper::appendMsg(ui->txtMain, type, data, maxCount, currentCount, clear, ui->ckShow->isChecked());
}

void frmWebClient::connected()
{
    isOk = true;
    ui->btnConnect->setText(tr("Disconnect"));
    append(2, tr("Server Connection"));
    append(4, QString(tr("Local Address: %1  Local Port: %2")).arg(socket->localAddress().toString()).arg(socket->localPort()));
    append(4, QString(tr("Remote Address: %1  Remote Port: %2")).arg(socket->peerAddress().toString()).arg(socket->peerPort()));
}

void frmWebClient::disconnected()
{
    isOk = false;
    ui->btnConnect->setText(tr("Connect"));
    append(2, tr("Server Disconnected"));
}

void frmWebClient::error()
{
    append(4, socket->errorString());
}

void frmWebClient::sendData(const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendWebClient) {
        buffer = QtHelperData::hexStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    if (AppConfig::AsciiWebClient) {
        socket->sendTextMessage(data);
    } else {
        socket->sendBinaryMessage(buffer);
    }

    append(0, data);
}

void frmWebClient::textFrameReceived(const QString &data, bool isLastFrame)
{
    QString buffer = data;
    append(1, buffer);

    //自动回复数据,可以回复的数据是以;隔开,每行可以带多个;所以这里不需要继续判断
    if (AppConfig::DebugWebClient) {
        int count = AppData::Keys.count();
        for (int i = 0; i < count; i++) {
            if (AppData::Keys.at(i) == buffer) {
                sendData(AppData::Values.at(i));
                break;
            }
        }
    }
}

void frmWebClient::binaryFrameReceived(const QByteArray &data, bool isLastFrame)
{
    QString buffer;
    if (AppConfig::HexReceiveWebClient) {
        buffer = QtHelperData::byteArrayToHexStr(data);
    } else {
        buffer = QString(data);
    }

    textFrameReceived(buffer, isLastFrame);
}

void frmWebClient::textMessageReceived(const QString &data)
{
    textFrameReceived(data, true);
}

void frmWebClient::binaryMessageReceived(const QByteArray &data)
{
    binaryFrameReceived(data, true);
}

void frmWebClient::on_btnConnect_clicked()
{
    if (ui->btnConnect->text() == "Connect") {
        QString url = QString("%1:%2").arg(AppConfig::WebServerIP).arg(AppConfig::WebServerPort);
        socket->abort();
        socket->open(QUrl(url));
    } else {
        socket->abort();
    }
}

void frmWebClient::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppData::saveData(data);
    on_btnClear_clicked();
}

void frmWebClient::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmWebClient::on_btnSend_clicked()
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
