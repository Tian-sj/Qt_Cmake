#ifndef SUPERADMINWINDOW_H
#define SUPERADMINWINDOW_H

#include <app/mainwindow.h>

namespace SurperAdmin {

namespace Ui {
class SurperAdminWindow;
}

class FramelessWidget;

class SurperAdminWindow : public MainWindow
{
    Q_OBJECT

public:
    explicit SurperAdminWindow(QWidget *parent = nullptr);
    ~SurperAdminWindow();

protected:
    void initUI();
    void initUIText();

private:
    Ui::SurperAdminWindow *ui;
};

}

#endif // SUPERADMINWINDOW_H
