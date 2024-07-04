#include <QApplication>
#include <app/logindialog.h>
#include <QProcess>
#include <memory>

const int RESTART_EXIT_CODE = 100;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int exitCode;

    LoginDialog *login = new LoginDialog();

    if (login->exec() == QDialog::Accepted) {
        auto m = login->get_windows();
        delete login;
        login = nullptr;
        m->show();
        exitCode = a.exec();
    } else {
        delete login;
        login = nullptr;
        return 0;
    }

    if (exitCode == RESTART_EXIT_CODE) {
        QProcess::startDetached(QApplication::applicationFilePath(), QStringList());
        return 0;
    }

    return exitCode;
}
