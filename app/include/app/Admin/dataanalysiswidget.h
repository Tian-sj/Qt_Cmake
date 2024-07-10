#ifndef DATAANALYSISWIDGET_H
#define DATAANALYSISWIDGET_H

#include <QWidget>
#include <QCalendarWidget>
#include <QPropertyAnimation>
#include <app/settingwidget.h>

namespace Admin {

namespace Ui {
class DataAnalysisWidget;
}

class DataAnalysisWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataAnalysisWidget(QWidget *parent = nullptr);
    ~DataAnalysisWidget();

private:
    void initUI();

    void initUIText();

    void initConnect();

    void initCalendarWidget(QCalendarWidget* calendar);

    void loadCSS(const QString &cssFile, QWidget* w = nullptr);

public slots:
    void windows_close();

private slots:
    void on_page_change(int index);
    void on_start_date_dateChanged(const QDate &date);

    void on_end_date_dateChanged(const QDate &date);

    void on_cmb_users_currentIndexChanged(int index);

    void on_cmb_series_currentIndexChanged(int index);

    void on_cmb_models_currentIndexChanged(int index);

    void on_cmb_devices_currentIndexChanged(int index);

    void on_btn_data_clicked();

private:
    Ui::DataAnalysisWidget *ui;

    QPropertyAnimation *animation_data;

    SettingWidget* data_widget;
};

}

#endif // DATAANALYSISWIDGET_H
