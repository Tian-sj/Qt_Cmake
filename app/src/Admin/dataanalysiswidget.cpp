#include <app/Admin/dataanalysiswidget.h>
#include <QFile>
#include <QTextCharFormat>
#include <QSpinBox>
#include <QToolButton>
#include <QListView>
#include <QRadioButton>

#include "ui_dataanalysiswidget.h"

namespace Admin {

DataAnalysisWidget::DataAnalysisWidget(QWidget *parent)
    : QWidget(parent)
    , animation_data(nullptr)
    , ui(new Ui::DataAnalysisWidget)
{
    ui->setupUi(this);

    initUI();
    initUIText();
    initConnect();
}

DataAnalysisWidget::~DataAnalysisWidget()
{
    delete data_widget;
    delete ui;
}

void DataAnalysisWidget::initUI()
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->cmb_users->setView(new QListView());
    ui->cmb_series->setView(new QListView());
    ui->cmb_models->setView(new QListView());
    ui->cmb_devices->setView(new QListView());
    loadCSS(":/css/Admin/DataAnalysisWidget.css");
    initCalendarWidget(ui->start_date->calendarWidget());
    initCalendarWidget(ui->end_date->calendarWidget());
    ui->start_date->setDateTime(QDateTime::currentDateTime().addDays(-1));
    ui->end_date->setDateTime(QDateTime::currentDateTime());
    ui->end_date->setMaximumDate(QDate::currentDate());
    ui->radioGroup->setId(ui->radio_table, 0);
    ui->radioGroup->setId(ui->radio_graphs, 1);

    ui->btn_data->setEnabled(false);

    data_widget = new SettingWidget(this);
    data_widget->setObjectName("SettingWidget");
    data_widget->raise();
    data_widget->setGeometry(0, 0, 0, 0);

    animation_data = new QPropertyAnimation(data_widget, "geometry");
    animation_data->setDuration(300);

    //test
    ui->cmb_devices->blockSignals(true);
    ui->cmb_devices->addItem("");
    ui->cmb_devices->addItem("测试");
    ui->cmb_devices->blockSignals(false);
}

void DataAnalysisWidget::initUIText()
{
    // ui->start_date->setLocale(QLocale(QLocale::Chinese));
    ui->start_date->setLocale(QLocale(QLocale::English));
    this->setWindowTitle(tr("Data Analysis"));
    ui->lab_users->setText(tr("Users"));
    ui->lab_series->setText(tr("Series"));
    ui->lab_models->setText(tr("Models"));
    ui->lab_devices->setText(tr("Devices"));
    ui->lab_data->setText(tr("Data"));
    ui->lab_start_date->setText(tr("Start Date"));
    ui->lab_end_date->setText(tr("End Date"));
    ui->btn_search->setText(tr("Search"));
    ui->btn_export->setText(tr("Export"));
    ui->radio_table->setText(tr("Table"));
    ui->radio_graphs->setText(tr("Graphs"));
}

void DataAnalysisWidget::initConnect()
{
    connect(ui->radioGroup, &QButtonGroup::idClicked, this, &DataAnalysisWidget::on_page_change);
}

void DataAnalysisWidget::initCalendarWidget(QCalendarWidget* calendar)
{
    QTextCharFormat format;
    format.setForeground(QColor(Qt::black));
    calendar->setWeekdayTextFormat(Qt::Saturday, format);
    calendar->setWeekdayTextFormat(Qt::Sunday, format);
    calendar->setFixedSize(500, 300);
    calendar->findChildren<QWidget*>().value(0)->setCursor(Qt::PointingHandCursor);
    calendar->findChildren<QSpinBox*>().value(0)->setAlignment(Qt::AlignCenter);
    calendar->findChildren<QSpinBox*>().value(0)->setButtonSymbols(QSpinBox::NoButtons);
    QList<QToolButton*> toolbtn_list = calendar->findChildren<QToolButton*>();
    for (int var = 0; var < toolbtn_list.size(); ++var) {
        toolbtn_list.value(var)->setCursor(Qt::PointingHandCursor);
    }
    QSize s = QSize(32, 32);
    QToolButton *toolbtn = calendar->findChild<QToolButton*>("qt_calendar_prevmonth");
    toolbtn->setIcon(QPixmap(":/images/left-arrow.png"));
    toolbtn->setIconSize(s);
    toolbtn = calendar->findChild<QToolButton*>("qt_calendar_nextmonth");
    toolbtn->setIcon(QPixmap(":/images/right-arrow.png"));
    toolbtn->setIconSize(s);
}

void DataAnalysisWidget::loadCSS(const QString &cssFile, QWidget* w)
{
    QFile file(cssFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    QTextStream stream(&file);
    QString styleSheet = stream.readAll();
    if (!w)
        this->setStyleSheet(styleSheet);
    else
        w->setStyleSheet(styleSheet);

    file.close();
}

void DataAnalysisWidget::windows_close()
{
    this->close();
}

void DataAnalysisWidget::on_page_change(int index)
{
    if (index != ui->stackedWidget->currentIndex()) {
        ui->stackedWidget->setCurrentIndex(index);
    }
}

void DataAnalysisWidget::on_start_date_dateChanged(const QDate &date)
{
    ui->end_date->setMinimumDate(date);
}

void DataAnalysisWidget::on_end_date_dateChanged(const QDate &date)
{
    ui->start_date->setMaximumDate(date);
}

void DataAnalysisWidget::on_cmb_users_currentIndexChanged(int index)
{

}

void DataAnalysisWidget::on_cmb_series_currentIndexChanged(int index)
{

}

void DataAnalysisWidget::on_cmb_models_currentIndexChanged(int index)
{

}

void DataAnalysisWidget::on_cmb_devices_currentIndexChanged(int index)
{
    if (index) {
        animation_data->setStartValue(QRect(0, 0, 500, 0));
        animation_data->setEndValue(QRect(0, 0, 500, 700));
        ui->btn_data->setEnabled(true);
    } else {
        ui->btn_data->setEnabled(false);
    }
}

void DataAnalysisWidget::on_btn_data_clicked()
{
    animation_data->start();
    // animation_data->setStartValue(data_widget->rect());
    // animation_data->setEndValue(QRect(ui->btn_data->x(), ui->btn_data->y() + ui->btn_data->height(), ui->btn_data->width(), 0));
}

}
