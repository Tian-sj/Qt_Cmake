#pragma once

#include "gui/main_object.hpp"

class QTimer;
class Cache;

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget, public MainObject
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void initUi() override;
    virtual void initConnect() override;
    virtual void initUiText() override;

private slots:
    void on_timer_rc();

private:
    Ui::MainWindow *ui;

    QTimer* timer_rc_;

    bool is_show_;
    bool is_current_show_;
};
