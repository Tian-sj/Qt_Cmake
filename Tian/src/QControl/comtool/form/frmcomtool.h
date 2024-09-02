#ifndef FRMCOMTOOL_H
#define FRMCOMTOOL_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui
{
class frmComTool;
}

class QTcpSocket;
class QextSerialPort;

class frmComTool : public QWidget
{
    Q_OBJECT

public:
    explicit frmComTool(QWidget *parent = 0);
    ~frmComTool();

    void initUIText();

    void _close();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::frmComTool *ui;

    bool comOk;
    QextSerialPort *com;
    QTimer *timerRead;
    QTimer *timerSend;
    QTimer *timerSave;

    int sleepTime;
    int sendCount;
    int receiveCount;
    bool isShow;

    bool tcpOk;
    QTcpSocket *socket;
    QTimer *timerConnect;

private slots:
    void initForm();
    void initConfig();
    void saveConfig();
    void readData();
    void sendData();
    void sendData(QString data);
    void saveData();

    void changeEnable(bool b);
    void append(int type, const QString &data, bool clear = false);

private slots:
    void connectNet();
    void readDataNet();
    void readErrorNet();

private slots:
    void on_btnOpen_clicked();
    void on_btnStopShow_clicked();
    void on_btnSendCount_clicked();
    void on_btnReceiveCount_clicked();

    void on_btnClear_clicked();
    void on_btnData_clicked();
    void on_btnStart_clicked();

    void on_ckAutoSend_stateChanged(int arg1);
    void on_ckAutoSave_stateChanged(int arg1);
};

#endif // FRMCOMTOOL_H
