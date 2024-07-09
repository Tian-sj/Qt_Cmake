#include <app/Admin/dataanalysiswidget.h>
#include <QFile>
#include <QTextCharFormat>
#include <QSpinBox>
#include <QToolButton>

#include "ui_dataanalysiswidget.h"

namespace Admin {

DataAnalysisWidget::DataAnalysisWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataAnalysisWidget)
{
    ui->setupUi(this);

    initUI();
    initUIText();
    initConnect();
}

DataAnalysisWidget::~DataAnalysisWidget()
{
    delete ui;
}

void DataAnalysisWidget::initUI()
{
    setAttribute(Qt::WA_DeleteOnClose);
    loadCSS(":/css/Admin/DataAnalysisWidget.css");
    initCalendarWidget(ui->start_date->calendarWidget());
    initCalendarWidget(ui->end_date->calendarWidget());
    ui->radioGroup->setId(ui->radio_table, 0);
    ui->radioGroup->setId(ui->radio_graphs, 1);
}

void DataAnalysisWidget::initUIText()
{
    // ui->start_date->setLocale(QLocale(QLocale::Chinese));
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

void DataAnalysisWidget::on_page_change(int index)
{
    if (index != ui->stackedWidget->currentIndex()) {
        ui->stackedWidget->setCurrentIndex(index);
    }
}

}
