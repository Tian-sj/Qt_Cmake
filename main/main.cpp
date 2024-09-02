#include <QApplication>
#include <QStyleFactory>
#include "qthelper.h"
#include <app/mainwindow.h>

int main(int argc, char *argv[])
{
    QtHelper::initMain();
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/main.ico"));

    QtHelper::initAll();

    MainWindow m;
    m.show();

    return a.exec();
}
