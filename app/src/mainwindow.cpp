#include <app/mainwindow.h>
#include <QFile>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    treeView = new QTreeView(this);
    model = new QStandardItemModel(this);
    treeView->setModel(model);

    
}

MainWindow::~MainWindow()
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
