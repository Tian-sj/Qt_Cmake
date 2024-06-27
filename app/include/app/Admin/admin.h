#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <app/mainwindow.h>

namespace Admin {

namespace Ui {
class AdminWindow;
}

class FramelessWidget;

class AdminWindow : public MainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

protected:
    void initUI();
    void initUIText();

private slots:
    void on_btn_max_clicked();

    void on_btn_max2_clicked();

private:
    void set_btn_max_visible(bool visible);

private:
    Ui::AdminWindow *ui;
};

}

#endif // ADMINWINDOW_H
