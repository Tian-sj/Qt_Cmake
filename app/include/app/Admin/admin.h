#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <app/mainwindow.h>
#include <app/Admin/deviceviewwidget.h>
#include <app/Admin/dataanalysiswidget.h>
#include <app/Admin/devicemanagementwidget.h>
#include <app/Admin/logmanagementwidget.h>
#include <app/Admin/usermanagementwidget.h>
#include <app/Admin/systemsettingwidget.h>

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

    void set_test();

protected:
    void initUI() override;
    void initUIText() override;
    void initConnect() override;

    void resizeEvent(QResizeEvent *event) override;

    void moveEvent(QMoveEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

signals:
    void _close_();

private slots:
    void on_btn_max_clicked();

    void on_btn_max2_clicked();

    void on_btn_change_user_clicked(bool checked);

    void on_act1_change_user_clicked();

    void on_act2_change_user_clicked();

    void on_gtoup_list_btn_clicked();

    void on_btn_setting_clicked();

    void on_setting_widget_close_clicked();

    void on_btn_close_clicked();

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

    DeviceViewWidget* DVW;
    DeviceManagementWidget* DMW;
    UserManagementWidget* UMW;
    SystemSettingWidget* SSW;
};

}

#endif // ADMINWINDOW_H
