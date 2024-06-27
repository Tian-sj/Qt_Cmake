#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <app/mainwindow.h>

namespace User {

namespace Ui {
class UserWindow;
}

class FramelessWidget;

class UserWindow : public MainWindow
{
    Q_OBJECT

public:
    explicit UserWindow(QWidget *parent = nullptr);
    ~UserWindow();

protected:
    void initUI();
    void initUIText();

private:
    Ui::UserWindow *ui;
};

}

#endif // USERWINDOW_H
