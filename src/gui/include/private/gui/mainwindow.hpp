#pragma once

#include "gui/mainobject.hpp"

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
    virtual void init_ui() override;
    virtual void init_connect() override;
    virtual void init_ui_text() override;

private slots:
    void on_timer_rc();

private:
    Ui::MainWindow *ui;

    QTimer* timer_rc;

    bool is_show;
    bool is_current_show;
};
