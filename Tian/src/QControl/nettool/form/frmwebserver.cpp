﻿#include "frmwebserver.h"
#include "ui_frmwebserver.h"
#include "qthelper.h"

frmWebServer::frmWebServer(QWidget *parent) : QWidget(parent), ui(new Ui::frmWebServer)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
    on_btnListen_clicked();
}

frmWebServer::~frmWebServer()
{
    delete ui;
}

void frmWebServer::initUIText()
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

bool frmWebServer::eventFilter(QObject *watched, QEvent *event)
{
    //双击清空
    if (watched == ui->txtMain->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            on_btnClear_clicked();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmWebServer::initForm()
{
    QFont font;
    font.setPixelSize(16);
    ui->txtMain->setFont(font);
    ui->txtMain->viewport()->installEventFilter(this);

    isOk = false;

    //实例化对象并绑定信号槽
    server = new WebServer("WebServer", QWebSocketServer::NonSecureMode, this);
    connect(server, SIGNAL(connected(QString, int)), this, SLOT(connected(QString, int)));
    connect(server, SIGNAL(disconnected(QString, int)), this, SLOT(disconnected(QString, int)));
    connect(server, SIGNAL(error(QString, int, QString)), this, SLOT(error(QString, int, QString)));
    connect(server, SIGNAL(sendData(QString, int, QString)), this, SLOT(sendData(QString, int, QString)));
    connect(server, SIGNAL(receiveData(QString, int, QString)), this, SLOT(receiveData(QString, int, QString)));

    //定时器发送数据
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    //填充数据到下拉框
    ui->cboxInterval->addItems(AppData::Intervals);
    ui->cboxData->addItems(AppData::Datas);
    QtHelper::initLocalIPs(ui->cboxListenIP, AppConfig::WebListenIP);
}

void frmWebServer::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendWebServer);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveWebServer);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiWebServer);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugWebServer);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendWebServer);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalWebServer)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxListenIP->setCurrentIndex(ui->cboxListenIP->findText(AppConfig::WebListenIP));
    connect(ui->cboxListenIP, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(AppConfig::WebListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->ckSelectAll->setChecked(AppConfig::SelectAllWebServer);
    connect(ui->ckSelectAll, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    this->initTimer();
}

void frmWebServer::saveConfig()
{
    AppConfig::HexSendWebServer = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveWebServer = ui->ckHexReceive->isChecked();
    AppConfig::AsciiWebServer = ui->ckAscii->isChecked();
    AppConfig::DebugWebServer = ui->ckDebug->isChecked();
    AppConfig::AutoSendWebServer = ui->ckAutoSend->isChecked();
    AppConfig::IntervalWebServer = ui->cboxInterval->currentText().toInt();
    AppConfig::WebListenIP = ui->cboxListenIP->currentText();
    AppConfig::WebListenPort = ui->txtListenPort->text().trimmed().toInt();
    AppConfig::SelectAllWebServer = ui->ckSelectAll->isChecked();
    AppConfig::writeConfig();

    this->initTimer();
}

void frmWebServer::initTimer()
{
    if (timer->interval() != AppConfig::IntervalWebServer) {
        timer->setInterval(AppConfig::IntervalWebServer);
    }

    if (AppConfig::AutoSendWebServer) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void frmWebServer::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;
    QtHelper::appendMsg(ui->txtMain, type, data, maxCount, currentCount, clear, ui->ckShow->isChecked());
}

void frmWebServer::connected(const QString &ip, int port)
{
    append(2, QString("[%1:%2] %3").arg(ip).arg(port).arg(tr("Client Online")));

    QString str = QString("%1:%2").arg(ip).arg(port);
    ui->listWidget->addItem(str);
    ui->labCount->setText(QString(tr("A total of %1 clients.")).arg(ui->listWidget->count()));
}

void frmWebServer::disconnected(const QString &ip, int port)
{
    append(2, QString("[%1:%2] %3").arg(ip).arg(port).arg(tr("Client Offline")));

    int row = -1;
    QString str = QString("%1:%2").arg(ip).arg(port);
    for (int i = 0; i < ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->text() == str) {
            row = i;
            break;
        }
    }

    delete ui->listWidget->takeItem(row);
    ui->labCount->setText(QString(tr("A total of %1 clients")).arg(ui->listWidget->count()));
}

void frmWebServer::error(const QString &ip, int port, const QString &error)
{
    append(4, QString("[%1:%2] %3").arg(ip).arg(port).arg(error));
}

void frmWebServer::sendData(const QString &ip, int port, const QString &data)
{
    append(0, QString("[%1:%2] %3").arg(ip).arg(port).arg(data));
}

void frmWebServer::receiveData(const QString &ip, int port, const QString &data)
{
    append(1, QString("[%1:%2] %3").arg(ip).arg(port).arg(data));
}

void frmWebServer::on_btnListen_clicked()
{
    if (ui->btnListen->text() == "Listening") {
        isOk = server->start();
        if (isOk) {
            append(2, tr("Listening Successful"));
            ui->btnListen->setText(tr("Close"));
        } else {
            append(3, QString(tr("Listening Failed: %1")).arg(server->errorString()));
        }
    } else {
        isOk = false;
        server->stop();
        ui->btnListen->setText(tr("Listening"));
    }
}

void frmWebServer::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppData::saveData(data);
    on_btnClear_clicked();
}

void frmWebServer::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmWebServer::on_btnSend_clicked()
{
    if (!isOk) {
        return;
    }

    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }

    if (ui->ckSelectAll->isChecked()) {
        server->writeData(data);
    } else {
        int row = ui->listWidget->currentRow();
        if (row >= 0) {
            QString str = ui->listWidget->item(row)->text();
            QStringList list = str.split(":");
            server->writeData(list.at(0), list.at(1).toInt(), data);
        }
    }
}

void frmWebServer::on_btnRemove_clicked()
{
    if (ui->ckSelectAll->isChecked()) {
        server->remove();
    } else {
        int row = ui->listWidget->currentRow();
        if (row >= 0) {
            QString str = ui->listWidget->item(row)->text();
            QStringList list = str.split(":");
            server->remove(list.at(0), list.at(1).toInt());
        }
    }
}
