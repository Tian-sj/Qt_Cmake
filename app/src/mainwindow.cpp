#include <app/mainwindow.h>
#include <FramelessWidget/FramelessWidget.h>
#include <app/SuperAdmin/superadmin.h>
#include <app/Admin/admin.h>
#include <app/User/user.h>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    frameless_widget(new FramelessWidget(this))
{
}

MainWindow::~MainWindow()
{
}

MainWindow *MainWindow::create(int role)
{
    switch (role) {
    case 0:
        return new SurperAdmin::SurperAdminWindow();
        break;
    case 1:
        return new Admin::AdminWindow();
        break;
    case 2:
        return new User::UserWindow();
        break;
    }
    return new MainWindow();
}

void MainWindow::initUI()
{

}

void MainWindow::initUIText()
{

}

void MainWindow::loadCSS(const QString &cssFile)
{
    QFile file(cssFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    QTextStream stream(&file);
    QString styleSheet = stream.readAll();
    this->setStyleSheet(styleSheet);

    file.close();
}
