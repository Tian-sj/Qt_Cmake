#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>

namespace Ui {
class frmMain;
}

class frmTcpClient;
class frmTcpServer;
class frmUdpClient;
class frmUdpServer;
class frmWebClient;
class frmWebServer;

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

    void _close();

    void initUIText();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::frmMain *ui;

    frmTcpClient* ftc;
    frmTcpServer* fts;
    frmUdpClient* fuc;
    frmUdpServer* fus;
    frmWebClient* fwc;
    frmWebServer* fws;

private slots:
    void initForm();
    void initConfig();
    void saveConfig();
};

#endif // FRMMAIN_H
