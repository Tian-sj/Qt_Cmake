#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

namespace Ui {
class MainWindow;
}

class FramelessWidget;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void init_widget();

private:
    Ui::MainWindow *ui;

    FramelessWidget* frameless_widget;
};

#endif // MAINWINDOW_H
