#include "frmmain.h"
#include <QApplication>

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    frmMain *w = new frmMain();
    w->setWindowIcon(QIcon(":/images/nettool.ico"));
    w->resize(950, 700);
    w->show();

    return a.exec();
}
