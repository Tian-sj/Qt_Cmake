#pragma once

#include "gui/base_window.hpp"

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow final : public BaseWindow<MainWindow, Ui::MainWindow, QMainWindow>
{
    Q_OBJECT

public:
    explicit MainWindow(bool use_reg_code, QWidget *parent = nullptr);
    ~MainWindow() override;

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
