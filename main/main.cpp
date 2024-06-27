#include <QApplication>
#include <app/logindialog.h>
#include <app/mainwindow.h>
#include <QProcess>

const int RESTART_EXIT_CODE = 100;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int exitCode;

    int role = -1;

    LoginDialog login(role);

    if (login.exec() == QDialog::Accepted) {
        auto m = MainWindow::create(role);
        m->show();
        exitCode = a.exec();
    } else {
        return 0;
    }

    if (exitCode == RESTART_EXIT_CODE) {
        QProcess::startDetached(QApplication::applicationFilePath(), QStringList());
        return 0;
    }

    return exitCode;
}
