#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>

namespace Ui {
class SettingWidget;
}

class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWidget(QWidget *parent = nullptr);
    ~SettingWidget();

    int getWidth();

protected:
    virtual void initUI();
    virtual void initUIText();

    virtual void loadCSS(const QString &cssFile);

public:
signals:
    void _close();

private slots:
    void on_setting_widget_btn_close_clicked();

    void on_language_activated(int index);

private:
    Ui::SettingWidget *ui;

    int m_width;
};

#endif // SETTINGWIDGET_H
