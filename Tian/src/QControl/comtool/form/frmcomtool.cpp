#include <QTcpSocket>
#include "qextserialport.h"
#include "frmcomtool.h"
#include "ui_frmcomtool.h"
#include "qthelper.h"
#include "qthelperdata.h"

frmComTool::frmComTool(QWidget *parent) : QWidget(parent), ui(new Ui::frmComTool)
{
    AppConfig::ConfigFile = QString("%1/%2.ini").arg(QtHelper::appPath()).arg(QtHelper::appName());
    AppConfig::readConfig();

    AppData::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000" << "3000" << "5000" << "10000";
    AppData::readSendData();
    AppData::readDeviceData();

    QtHelper::setFormInCenter(this);

    ui->setupUi(this);
    this->initForm();
    this->initConfig();
    this->initUIText();
    QtHelper::setFormInCenter(this);
}

frmComTool::~frmComTool()
{
    delete ui;
}

void frmComTool::initUIText()
{
    this->setWindowTitle(tr("Serial Port Debug Assistant"));
    ui->labPortName->setText(tr("Serial Port"));
    ui->labBaudRate->setText(tr("Baud Rate"));
    ui->labDataBit->setText(tr("Data Bits"));
    ui->labParity->setText(tr("Parity"));
    ui->labStopBit->setText(tr("Stop Bits"));
    ui->btnOpen->setText(tr("Open Serial Port"));
    ui->ckHexSend->setText(tr("Hex Send"));
    ui->ckHexReceive->setText(tr("Hex Recv"));
    ui->ckDebug->setText(tr("Emu Device"));
    ui->ckAutoClear->setText(tr("Auto Clear"));
    ui->ckAutoSend->setText(tr("Auto Send"));
    ui->ckAutoSave->setText(tr("Auto Save"));
    ui->btnSendCount->setText(tr("Sent: 0 Bytes"));
    ui->btnReceiveCount->setText(tr("Received: 0 Bytes"));
    ui->btnStopShow->setText(tr("Stop Show"));
    ui->btnSave->setText(tr("Save Data"));
    ui->btnData->setText(tr("Manage Data"));
    ui->btnClear->setText(tr("Clear Data"));
    ui->btnSend->setText(tr("Send"));
    ui->tabWidget->setTabText(0, tr("Serial Config"));
    ui->tabWidget->setTabText(1, tr("Network Config"));
    ui->labMode->setText(tr("Conversion"));
    ui->labServerIP->setText(tr("Remote Addr"));
    ui->labServerPort->setText(tr("Remote Port"));
    ui->labListenPort->setText(tr("Listen Port"));
    ui->labSleepTime->setText(tr("Delay Time"));
    ui->ckAutoConnect->setText(tr("Auto Reconnect"));
    ui->btnStart->setText(tr("Start"));

    QStringList parityList;
    parityList << tr("None") << tr("Odd") << tr("Even");
#ifdef Q_OS_WIN
    parityList << tr("Mark");
#endif
    parityList << tr("Space");

    ui->cboxParity->blockSignals(true);
    ui->cboxParity->addItems(parityList);
    ui->cboxParity->setCurrentIndex(ui->cboxParity->findText(AppConfig::Parity));
    ui->cboxParity->blockSignals(false);
}

void frmComTool::_close()
{
    this->hide();
    this->deleteLater();
}

void frmComTool::closeEvent(QCloseEvent *event)
{
    _close();
    event->ignore();
}

void frmComTool::initForm()
{
    comOk = false;
    com = 0;
    sleepTime = 10;
    receiveCount = 0;
    sendCount = 0;
    isShow = true;

    ui->cboxSendInterval->addItems(AppData::Intervals);
    ui->cboxData->addItems(AppData::Datas);

    //读取数据
    timerRead = new QTimer(this);
    timerRead->setInterval(100);
    connect(timerRead, SIGNAL(timeout()), this, SLOT(readData()));

    //发送数据
    timerSend = new QTimer(this);
    connect(timerSend, SIGNAL(timeout()), this, SLOT(sendData()));
    connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(sendData()));

    //保存数据
    timerSave = new QTimer(this);
    connect(timerSave, SIGNAL(timeout()), this, SLOT(saveData()));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(saveData()));

    ui->tabWidget->setCurrentIndex(0);
    changeEnable(false);

    tcpOk = false;
    socket = new QTcpSocket(this);
    socket->abort();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readDataNet()));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(readErrorNet()));
#else
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(readErrorNet()));
#endif

    timerConnect = new QTimer(this);
    connect(timerConnect, SIGNAL(timeout()), this, SLOT(connectNet()));
    timerConnect->setInterval(3000);
    timerConnect->start();

#ifdef __arm__
    ui->widgetRight->setFixedWidth(280);
#endif
}

void frmComTool::initConfig()
{
    QStringList comList;
    for (int i = 1; i <= 20; i++) {
        comList << QString("COM%1").arg(i);
    }

    comList << "ttyUSB0" << "ttyS0" << "ttyS1" << "ttyS2" << "ttyS3" << "ttyS4";
    comList << "ttymxc1" << "ttymxc2" << "ttymxc3" << "ttymxc4";
    comList << "ttySAC1" << "ttySAC2" << "ttySAC3" << "ttySAC4";
    ui->cboxPortName->addItems(comList);
    ui->cboxPortName->setCurrentIndex(ui->cboxPortName->findText(AppConfig::PortName));
    connect(ui->cboxPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList baudList;
    baudList << "50" << "75" << "100" << "134" << "150" << "200" << "300" << "600" << "1200"
             << "1800" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400"
             << "56000" << "57600" << "76800" << "115200" << "128000" << "256000";

    ui->cboxBaudRate->addItems(baudList);
    ui->cboxBaudRate->setCurrentIndex(ui->cboxBaudRate->findText(QString::number(AppConfig::BaudRate)));
    connect(ui->cboxBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList dataBitsList;
    dataBitsList << "5" << "6" << "7" << "8";

    ui->cboxDataBit->addItems(dataBitsList);
    ui->cboxDataBit->setCurrentIndex(ui->cboxDataBit->findText(QString::number(AppConfig::DataBit)));
    connect(ui->cboxDataBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    connect(ui->cboxParity, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList stopBitsList;
    stopBitsList << "1";
#ifdef Q_OS_WIN
    stopBitsList << "1.5";
#endif
    stopBitsList << "2";

    ui->cboxStopBit->addItems(stopBitsList);
    ui->cboxStopBit->setCurrentIndex(ui->cboxStopBit->findText(QString::number(AppConfig::StopBit)));
    connect(ui->cboxStopBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexSend->setChecked(AppConfig::HexSend);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceive);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::Debug);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoClear->setChecked(AppConfig::AutoClear);
    connect(ui->ckAutoClear, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSend);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSave->setChecked(AppConfig::AutoSave);
    connect(ui->ckAutoSave, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    QStringList sendInterval;
    QStringList saveInterval;
    sendInterval << "100" << "300" << "500";

    for (int i = 1000; i <= 10000; i = i + 1000) {
        sendInterval << QString::number(i);
        saveInterval << QString::number(i);
    }

    ui->cboxSendInterval->addItems(sendInterval);
    ui->cboxSaveInterval->addItems(saveInterval);

    ui->cboxSendInterval->setCurrentIndex(ui->cboxSendInterval->findText(QString::number(AppConfig::SendInterval)));
    connect(ui->cboxSendInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
    ui->cboxSaveInterval->setCurrentIndex(ui->cboxSaveInterval->findText(QString::number(AppConfig::SaveInterval)));
    connect(ui->cboxSaveInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    timerSend->setInterval(AppConfig::SendInterval);
    timerSave->setInterval(AppConfig::SaveInterval);

    if (AppConfig::AutoSend) {
        timerSend->start();
    }

    if (AppConfig::AutoSave) {
        timerSave->start();
    }

    //串口转网络部分
    ui->cboxMode->setCurrentIndex(ui->cboxMode->findText(AppConfig::Mode));
    connect(ui->cboxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(AppConfig::ServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(AppConfig::ServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(AppConfig::ListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    QStringList values;
    values << "0" << "10" << "50";

    for (int i = 100; i < 1000; i = i + 100) {
        values << QString("%1").arg(i);
    }

    ui->cboxSleepTime->addItems(values);

    ui->cboxSleepTime->setCurrentIndex(ui->cboxSleepTime->findText(QString::number(AppConfig::SleepTime)));
    connect(ui->cboxSleepTime, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoConnect->setChecked(AppConfig::AutoConnect);
    connect(ui->ckAutoConnect, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
}

void frmComTool::saveConfig()
{
    AppConfig::PortName = ui->cboxPortName->currentText();
    AppConfig::BaudRate = ui->cboxBaudRate->currentText().toInt();
    AppConfig::DataBit = ui->cboxDataBit->currentText().toInt();
    AppConfig::Parity = ui->cboxParity->currentText();
    AppConfig::StopBit = ui->cboxStopBit->currentText().toDouble();

    AppConfig::HexSend = ui->ckHexSend->isChecked();
    AppConfig::HexReceive = ui->ckHexReceive->isChecked();
    AppConfig::Debug = ui->ckDebug->isChecked();
    AppConfig::AutoClear = ui->ckAutoClear->isChecked();

    AppConfig::AutoSend = ui->ckAutoSend->isChecked();
    AppConfig::AutoSave = ui->ckAutoSave->isChecked();

    int sendInterval = ui->cboxSendInterval->currentText().toInt();
    if (sendInterval != AppConfig::SendInterval) {
        AppConfig::SendInterval = sendInterval;
        timerSend->setInterval(AppConfig::SendInterval);
    }

    int saveInterval = ui->cboxSaveInterval->currentText().toInt();
    if (saveInterval != AppConfig::SaveInterval) {
        AppConfig::SaveInterval = saveInterval;
        timerSave->setInterval(AppConfig::SaveInterval);
    }

    AppConfig::Mode = ui->cboxMode->currentText();
    AppConfig::ServerIP = ui->txtServerIP->text().trimmed();
    AppConfig::ServerPort = ui->txtServerPort->text().toInt();
    AppConfig::ListenPort = ui->txtListenPort->text().toInt();
    AppConfig::SleepTime = ui->cboxSleepTime->currentText().toInt();
    AppConfig::AutoConnect = ui->ckAutoConnect->isChecked();

    AppConfig::writeConfig();
}

void frmComTool::changeEnable(bool b)
{
    ui->cboxBaudRate->setEnabled(!b);
    ui->cboxDataBit->setEnabled(!b);
    ui->cboxParity->setEnabled(!b);
    ui->cboxPortName->setEnabled(!b);
    ui->cboxStopBit->setEnabled(!b);
    ui->btnSend->setEnabled(b);
    ui->ckAutoSend->setEnabled(b);
    ui->ckAutoSave->setEnabled(b);
}

void frmComTool::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;

    if (clear) {
        ui->txtMain->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }

    if (!isShow) {
        return;
    }

    //过滤回车换行符
    QString strData = data;
    strData = strData.replace("\r", "");
    strData = strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = tr("Serial Send >>");
        ui->txtMain->setTextColor(QColor("dodgerblue"));
    } else if (type == 1) {
        strType = tr("Serial Receive <<");
        ui->txtMain->setTextColor(QColor("red"));
    } else if (type == 2) {
        strType = tr("Processing Delay >>");
        ui->txtMain->setTextColor(QColor("gray"));
    } else if (type == 3) {
        strType = tr("Verifying >>");
        ui->txtMain->setTextColor(QColor("green"));
    } else if (type == 4) {
        strType = tr("Network Send >>");
        ui->txtMain->setTextColor(QColor(24, 189, 155));
    } else if (type == 5) {
        strType = tr("Network Receive <<");
        ui->txtMain->setTextColor(QColor(255, 107, 107));
    } else if (type == 6) {
        strType = tr("Notification >>");
    }

    strData = QString(tr("Time[%1] %2 %3")).arg(TIMEMS).arg(strType).arg(strData);
    ui->txtMain->append(strData);
    currentCount++;
}

void frmComTool::readData()
{
    if (com->bytesAvailable() <= 0) {
        return;
    }

    QtHelper::sleep(sleepTime);
    QByteArray data = com->readAll();
    int dataLen = data.length();
    if (dataLen <= 0) {
        return;
    }

    if (isShow) {
        QString buffer;
        if (ui->ckHexReceive->isChecked()) {
            buffer = QtHelperData::byteArrayToHexStr(data);
        } else {
            //buffer = QtHelperData::byteArrayToAsciiStr(data);
            buffer = QString::fromLocal8Bit(data);
        }

        //启用调试则模拟调试数据
        if (ui->ckDebug->isChecked()) {
            int count = AppData::Keys.count();
            for (int i = 0; i < count; i++) {
                if (buffer.startsWith(AppData::Keys.at(i))) {
                    sendData(AppData::Values.at(i));
                    break;
                }
            }
        }

        append(1, buffer);
        receiveCount = receiveCount + data.size();
        ui->btnReceiveCount->setText(QString(tr("Received: %1 Bytes")).arg(receiveCount));

        //启用网络转发则调用网络发送数据
        if (tcpOk) {
            socket->write(data);
            append(4, QString(buffer));
        }
    }
}

void frmComTool::sendData()
{
    QString str = ui->cboxData->currentText();
    if (str.isEmpty()) {
        ui->cboxData->setFocus();
        return;
    }

    sendData(str);

    if (ui->ckAutoClear->isChecked()) {
        ui->cboxData->setCurrentIndex(-1);
        ui->cboxData->setFocus();
    }
}

void frmComTool::sendData(QString data)
{
    if (com == 0 || !com->isOpen()) {
        return;
    }

    //短信猫调试
    if (data.startsWith("AT")) {
        data += "\r";
    }

    QByteArray buffer;
    if (ui->ckHexSend->isChecked()) {
        buffer = QtHelperData::hexStrToByteArray(data);
    } else {
        buffer = QtHelperData::asciiStrToByteArray(data);
    }

    com->write(buffer);
    append(0, data);
    sendCount = sendCount + buffer.size();
    ui->btnSendCount->setText(QString(tr("Sent: %1 Bytes")).arg(sendCount));
}

void frmComTool::saveData()
{
    QString tempData = ui->txtMain->toPlainText();
    if (tempData.isEmpty()) {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    QString name = now.toString("yyyy-MM-dd-HH-mm-ss");
    QString fileName = QString("%1/%2.txt").arg(QtHelper::appPath()).arg(name);

    QFile file(fileName);
    file.open(QFile::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << tempData;
    file.close();

    on_btnClear_clicked();
}

void frmComTool::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "Open Serial Port") {
        com = new QextSerialPort(ui->cboxPortName->currentText(), QextSerialPort::Polling);
        comOk = com->open(QIODevice::ReadWrite);

        if (comOk) {
            // Clear the buffer
            com->flush();
            // Set baud rate
            com->setBaudRate((BaudRateType)ui->cboxBaudRate->currentText().toInt());
            // Set data bits
            com->setDataBits((DataBitsType)ui->cboxDataBit->currentText().toInt());
            // Set parity
            com->setParity((ParityType)ui->cboxParity->currentIndex());
            // Set stop bits
            com->setStopBits((StopBitsType)ui->cboxStopBit->currentIndex());
            com->setFlowControl(FLOW_OFF);
            com->setTimeout(10);

            changeEnable(true);
            ui->btnOpen->setText(tr("Close Serial Port"));
            timerRead->start();
        }
    } else {
        timerRead->stop();
        com->close();
        com->deleteLater();

        changeEnable(false);
        ui->btnOpen->setText(tr("Open Serial Port"));
        on_btnClear_clicked();
        comOk = false;
    }
}

void frmComTool::on_btnSendCount_clicked()
{
    sendCount = 0;
    ui->btnSendCount->setText(tr("Sent: 0 Bytes"));
}

void frmComTool::on_btnReceiveCount_clicked()
{
    receiveCount = 0;
    ui->btnReceiveCount->setText(tr("Received: 0 Bytes"));
}

void frmComTool::on_btnStopShow_clicked()
{
    if (ui->btnStopShow->text() == "Stop Show") {
        isShow = false;
        ui->btnStopShow->setText(tr("Start Show"));
    } else {
        isShow = true;
        ui->btnStopShow->setText(tr("Stop Show"));
    }
}

void frmComTool::on_btnData_clicked()
{
    QString fileName = QString("%1/%2").arg(QtHelper::appPath()).arg("send.txt");
    QFile file(fileName);
    if (!file.exists()) {
        return;
    }


    if (ui->btnData->text() == "Manage Data") {
        ui->txtMain->setReadOnly(false);
        ui->txtMain->clear();
        file.open(QFile::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        ui->txtMain->setText(in.readAll());
        file.close();
        ui->btnSave->setText(tr("Save Data"));
    } else {
        ui->txtMain->setReadOnly(true);
        file.open(QFile::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << ui->txtMain->toPlainText();
        file.close();
        ui->txtMain->clear();
        ui->btnData->setText(tr("Manage Data"));
        AppData::readSendData();
    }
}

void frmComTool::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmComTool::on_btnStart_clicked()
{
    if (ui->btnStart->text() == tr("Start")) {
        if (AppConfig::ServerIP == "" || AppConfig::ServerPort == 0) {
            append(6, tr("IP Address and Remote Port cannot be empty"));
            return;
        }

        socket->connectToHost(AppConfig::ServerIP, AppConfig::ServerPort);
        if (socket->waitForConnected(100)) {
            ui->btnStart->setText(tr("Stop"));
            append(6, tr("Successfully connected to server"));
            tcpOk = true;
        }
    } else {
        socket->disconnectFromHost();
        if (socket->state() == QAbstractSocket::UnconnectedState || socket->waitForDisconnected(100)) {
            ui->btnStart->setText(tr("Start"));
            append(6, tr("Successfully disconnected from server"));
            tcpOk = false;
        }
    }
}

void frmComTool::on_ckAutoSend_stateChanged(int arg1)
{
    if (arg1 == 0) {
        ui->cboxSendInterval->setEnabled(false);
        timerSend->stop();
    } else {
        ui->cboxSendInterval->setEnabled(true);
        timerSend->start();
    }
}

void frmComTool::on_ckAutoSave_stateChanged(int arg1)
{
    if (arg1 == 0) {
        ui->cboxSaveInterval->setEnabled(false);
        timerSave->stop();
    } else {
        ui->cboxSaveInterval->setEnabled(true);
        timerSave->start();
    }
}

void frmComTool::connectNet()
{
    if (!tcpOk && AppConfig::AutoConnect && ui->btnStart->text() == tr("Start")) {
        if (AppConfig::ServerIP != "" && AppConfig::ServerPort != 0) {
            socket->connectToHost(AppConfig::ServerIP, AppConfig::ServerPort);
            if (socket->waitForConnected(100)) {
                ui->btnStart->setText(tr("Stop"));
                append(6, tr("Successfully connected to server"));
                tcpOk = true;
            }
        }
    }
}

void frmComTool::readDataNet()
{
    if (socket->bytesAvailable() > 0) {
        QtHelper::sleep(AppConfig::SleepTime);
        QByteArray data = socket->readAll();

        QString buffer;
        if (ui->ckHexReceive->isChecked()) {
            buffer = QtHelperData::byteArrayToHexStr(data);
        } else {
            buffer = QtHelperData::byteArrayToAsciiStr(data);
        }

        append(5, buffer);

        //将收到的网络数据转发给串口
        if (comOk) {
            com->write(data);
            append(0, buffer);
        }
    }
}

void frmComTool::readErrorNet()
{
    ui->btnStart->setText(tr("Start"));
    append(6, QString(tr("Failed to connect to server, %1")).arg(socket->errorString()));
    socket->disconnectFromHost();
    tcpOk = false;
}
