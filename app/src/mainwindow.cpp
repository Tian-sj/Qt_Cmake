#include <app/mainwindow.h>
#include <FramelessWidget/FramelessWidget.h>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    frameless_widget(new FramelessWidget(this)),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init_widget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_widget()
{
}
