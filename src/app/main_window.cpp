#include "main_window.h"

#include "./ui_main_window.h"
#include "cout_hello.h"

main_window::main_window(QWidget* parent)
        : QMainWindow(parent), ui(new Ui::main_window) {
    ui->setupUi(this);
}

main_window::~main_window() {
    delete ui;
}

void main_window::on_pushButton_clicked() {
    ui->textEdit->setText(QString::fromStdString(m_cout()).toUtf8());
}
