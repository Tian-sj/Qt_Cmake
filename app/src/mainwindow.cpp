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

std::unique_ptr<MainWindow> MainWindow::create(int role, QString username)
{
    switch (role) {
    case 0:
        return std::make_unique<SurperAdmin::SurperAdminWindow>();
    case 1:
        return std::make_unique<Admin::AdminWindow>();
    case 2:
        return std::make_unique<User::UserWindow>(username);
    default:
        return std::make_unique<MainWindow>();
    }
}

void MainWindow::initUI()
{
}

void MainWindow::initUIText()
{
}

void MainWindow::initConnect()
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
