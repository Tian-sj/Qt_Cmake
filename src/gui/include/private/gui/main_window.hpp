#pragma once

#include "gui/base_window.hpp"

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public BaseWindow<MainWindow, Ui::MainWindow, QMainWindow>
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initUi() override;
    void initConnect() override;
    void initUiText() override;
    void initLazy() override;

private slots:
    void timerRc();

private:
    // Ui::MainWindow ui;
    QTimer* timer_rc_;

    bool is_show_;
    bool is_current_show_;
};
