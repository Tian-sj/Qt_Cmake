#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <app/mainwindow.h>

#include <QMenu>
#include <QButtonGroup>
#include <QPropertyAnimation>

class SettingWidget;
class FramelessWidget;

namespace Admin {

namespace Ui {
class AdminWindow;
}

class AdminWindow : public MainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

protected:
    void initUI();
    void initUIText();
    void initConnect();

    void resizeEvent(QResizeEvent *event) override;

    void moveEvent(QMoveEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void on_btn_max_clicked();

    void on_btn_max2_clicked();

    void on_btn_change_user_clicked(bool checked);

    void on_act1_change_user_clicked();

    void on_act2_change_user_clicked();

    void on_gtoup_list_btn_clicked();

    void on_btn_setting_clicked();

    void on_setting_widget_close_clicked();

private:
    void set_btn_max_visible(bool visible);

    void updateChildWindowGeometry();

private:
    Ui::AdminWindow *ui;

    QMenu *menu_change_user;
    QAction* act1_change_user;
    QAction* act2_change_user;

    enum class BtnGroupType {
        DeviceView,
        DataAnalysis,
        DeviceManagement,
        LogManagement,
        UserManagement,
        SystemSetting
    };

    QMap<QString, BtnGroupType> btn_group_map;

    SettingWidget* set_widget;
    QPropertyAnimation *animation_start;
    QPropertyAnimation *animation_stop;

    bool is_show_setting_widget;
};

}

#endif // ADMINWINDOW_H
