#ifndef DEVICEVIEWWIDGET_H
#define DEVICEVIEWWIDGET_H

#include <QWidget>
#include <QStandardItem>
#include <QStandardItemModel>

namespace Admin {

namespace Ui {
class DeviceViewWidget;
}

class DeviceViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceViewWidget(QWidget *parent = nullptr);
    ~DeviceViewWidget();

private slots:
    void on_DVW_btn_show_hide_clicked();

private:
    void initUI();

    void initUIText();

    void initConnect();

    void loadCSS(const QString &cssFile, QWidget* w = nullptr);

    void set_DVW_btn_show_hide_state(bool state);

private:
    Ui::DeviceViewWidget *ui;

    bool is_DVW_btn_show_hide_state;  // show:true, hide:false
    QStandardItemModel *model;
};

}

#endif // DEVICEVIEWWIDGET_H
