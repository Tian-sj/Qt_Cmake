#include <QApplication>
#include <app/logindialog.h>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginDialog login;

    login.show();
    return a.exec();
}
