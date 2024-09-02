#include "frmmain.h"
#include "ui_frmmain.h"
#include "qthelper.h"

#include "frmtcpclient.h"
#include "frmtcpserver.h"
#include "frmudpclient.h"
#include "frmudpserver.h"
#include "frmwebclient.h"
#include "frmwebserver.h"

frmMain::frmMain(QWidget *parent)
    : QWidget(parent)
    , ftc(nullptr)
    , fts(nullptr)
    , fuc(nullptr)
    , fus(nullptr)
    , fwc(nullptr)
    , fws(nullptr)
    , ui(new Ui::frmMain)
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
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::_close()
{
    this->hide();
    this->deleteLater();
}

void frmMain::initUIText()
{
    this->setWindowTitle(tr("Network Debugging Assistant"));
    ftc->initUIText();
    fts->initUIText();
    fuc->initUIText();
    fus->initUIText();
    fwc->initUIText();
    fws->initUIText();
    ui->tabWidget->setTabText(0, tr("TCP Client"));
    ui->tabWidget->setTabText(1, tr("TCP Server"));
    ui->tabWidget->setTabText(2, tr("UDP Client"));
    ui->tabWidget->setTabText(3, tr("UDP Server"));
    ui->tabWidget->setTabText(4, tr("Web Client"));
    ui->tabWidget->setTabText(5, tr("Web Server"));
}
void frmMain::closeEvent(QCloseEvent *event)
{
    _close();
}

void frmMain::initForm()
{
    ftc = new frmTcpClient;
    fts = new frmTcpServer;
    fuc = new frmUdpClient;
    fus = new frmUdpServer;
    fwc = new frmWebClient;
    fws = new frmWebServer;

    ui->tabWidget->addTab(ftc, "");
    ui->tabWidget->addTab(fts, "");
    ui->tabWidget->addTab(fus, "");
    ui->tabWidget->addTab(fuc, "");
    ui->tabWidget->addTab(fwc, "");
    ui->tabWidget->addTab(fws, "");
#ifdef Q_OS_WASM
    AppConfig::CurrentIndex = 4;
#endif
}

void frmMain::initConfig()
{
    ui->tabWidget->setCurrentIndex(AppConfig::CurrentIndex);
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(saveConfig()));
}

void frmMain::saveConfig()
{
    AppConfig::CurrentIndex = ui->tabWidget->currentIndex();
    AppConfig::writeConfig();
}
