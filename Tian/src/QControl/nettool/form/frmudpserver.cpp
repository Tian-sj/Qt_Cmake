﻿#include "frmudpserver.h"
#include "ui_frmudpserver.h"
#include "qthelper.h"
#include "qthelperdata.h"

frmUdpServer::frmUdpServer(QWidget *parent) : QWidget(parent), ui(new Ui::frmUdpServer)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
    on_btnListen_clicked();
}

frmUdpServer::~frmUdpServer()
{
    delete ui;
}

void frmUdpServer::initUIText()
{
    ui->ckHexReceive->setText(tr("Hex Receive"));  // 16进制接收
    ui->ckHexSend->setText(tr("Hex Send"));  // 16进制发送
    ui->ckAscii->setText(tr("Control Chars"));  // 控制字符
    ui->ckShow->setText(tr("Pause Display"));  // 暂停显示
    ui->ckDebug->setText(tr("Simulate Device"));  // 模拟设备
    ui->ckAutoSend->setText(tr("Auto Send"));  // 定时发送
    ui->labListenIP->setText(tr("Listen IP"));  // 监听地址
    ui->labListenPort->setText(tr("Listen Port"));  // 监听端口
    ui->btnListen->setText(tr("Listen"));  // 监听
    ui->labCount->setText(tr("Total 0 Clients"));  // 共 0 个客户端
    ui->ckSelectAll->setText(tr("Send to All Clients"));  // 对所有客户端发送
    ui->btnSave->setText(tr("Save"));  // 保存
    ui->btnClear->setText(tr("Clear"));  // 清空
    ui->btnSend->setText(tr("Send"));  // 发送
    ui->btnRemove->setText(tr("Remove")); // 移除
}

bool frmUdpServer::eventFilter(QObject *watched, QEvent *event)
{
    //双击清空
    if (watched == ui->txtMain->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            on_btnClear_clicked();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmUdpServer::initForm()
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
    QtHelper::initLocalIPs(ui->cboxListenIP, AppConfig::UdpListenIP);
}

void frmUdpServer::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendUdpServer);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveUdpServer);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiUdpServer);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugUdpServer);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendUdpServer);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalUdpServer)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxListenIP->setCurrentIndex(ui->cboxListenIP->findText(AppConfig::UdpListenIP));
    connect(ui->cboxListenIP, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(AppConfig::UdpListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->ckSelectAll->setChecked(AppConfig::SelectAllUdpServer);
    connect(ui->ckSelectAll, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    this->initTimer();
}

void frmUdpServer::saveConfig()
{
    AppConfig::HexSendUdpServer = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveUdpServer = ui->ckHexReceive->isChecked();
    AppConfig::AsciiUdpServer = ui->ckAscii->isChecked();
    AppConfig::DebugUdpServer = ui->ckDebug->isChecked();
    AppConfig::AutoSendUdpServer = ui->ckAutoSend->isChecked();
    AppConfig::IntervalUdpServer = ui->cboxInterval->currentText().toInt();
    AppConfig::UdpListenIP = ui->cboxListenIP->currentText();
    AppConfig::UdpListenPort = ui->txtListenPort->text().trimmed().toInt();
    AppConfig::SelectAllUdpServer = ui->ckSelectAll->isChecked();
    AppConfig::writeConfig();

    this->initTimer();
}

void frmUdpServer::initTimer()
{
    if (timer->interval() != AppConfig::IntervalUdpServer) {
        timer->setInterval(AppConfig::IntervalUdpServer);
    }

    if (AppConfig::AutoSendUdpServer) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void frmUdpServer::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;
    QtHelper::appendMsg(ui->txtMain, type, data, maxCount, currentCount, clear, ui->ckShow->isChecked());
}

void frmUdpServer::error()
{
    append(3, socket->errorString());
}

void frmUdpServer::readData()
{
    QHostAddress host;
    quint16 port;
    QByteArray data;
    QString buffer;

    while (socket->hasPendingDatagrams()) {
        data.resize(socket->pendingDatagramSize());
        socket->readDatagram(data.data(), data.size(), &host, &port);

        if (AppConfig::HexReceiveUdpServer) {
            buffer = QtHelperData::byteArrayToHexStr(data);
        } else if (AppConfig::AsciiUdpServer) {
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

        //先过滤重复的
        str = QString("%1:%2").arg(ip).arg(port);
        for (int i = 0; i < ui->listWidget->count(); i++) {
            QString s = ui->listWidget->item(i)->text();
            if (str == s) {
                return;
            }
        }

        //添加到列表
        ui->listWidget->addItem(str);
        ui->labCount->setText(QString(tr("A total of %1 clients.")).arg(ui->listWidget->count()));

        if (AppConfig::DebugUdpServer) {
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

void frmUdpServer::sendData(const QString &ip, int port, const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendUdpServer) {
        buffer = QtHelperData::hexStrToByteArray(data);
    } else if (AppConfig::AsciiUdpServer) {
        buffer = QtHelperData::asciiStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    socket->writeDatagram(buffer, QHostAddress(ip), port);

    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    append(0, str);
}

void frmUdpServer::on_btnListen_clicked()
{
    if (ui->btnListen->text() == "Listening") {
        bool ok = socket->bind(QHostAddress(AppConfig::UdpListenIP), AppConfig::UdpListenPort);
        if (ok) {
            append(2, tr("Listening Successful"));
            ui->btnListen->setText(tr("Close"));
        } else {
            append(3, QString(tr("Listening Failed: %1")).arg(socket->errorString()));
        }
    } else {
        socket->abort();
        ui->btnListen->setText(tr("Listening"));
    }
}

void frmUdpServer::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppData::saveData(data);
    on_btnClear_clicked();
}

void frmUdpServer::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmUdpServer::on_btnSend_clicked()
{
    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }

    if (ui->ckSelectAll->isChecked()) {
        for (int i = 0; i < ui->listWidget->count(); i++) {
            QString str = ui->listWidget->item(i)->text();
            QStringList list = str.split(":");
            sendData(list.at(0), list.at(1).toInt(), data);
        }
    } else {
        int row = ui->listWidget->currentRow();
        if (row >= 0) {
            QString str = ui->listWidget->item(row)->text();
            QStringList list = str.split(":");
            sendData(list.at(0), list.at(1).toInt(), data);
        }
    }
}

void frmUdpServer::on_btnRemove_clicked()
{
    if (ui->ckSelectAll->isChecked()) {
        ui->listWidget->clear();
    } else {
        int row = ui->listWidget->currentRow();
        if (row >= 0) {
            delete ui->listWidget->takeItem(row);
        }
    }
}
