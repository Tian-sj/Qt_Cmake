#include <app/settingwidget.h>
#include <QFile>

#include "ui_settingwidget.h"

SettingWidget::SettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidget)
{
    ui->setupUi(this);

    initUI();
    initUIText();
}

SettingWidget::~SettingWidget()
{
}

int SettingWidget::getWidth()
{
    return m_width;
}

void SettingWidget::initUI()
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    loadCSS(":/css/SettingWidget.css");

    m_width = this->width();
}

void SettingWidget::initUIText()
{
    ui->lab_language->setText(tr("Language"));
    ui->language->addItem("English");
    ui->language->addItem("简体中文");
}

void SettingWidget::loadCSS(const QString &cssFile)
{
    QFile file(cssFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    QTextStream stream(&file);
    QString styleSheet = stream.readAll();
    this->setStyleSheet(styleSheet);

    file.close();
}

void SettingWidget::on_setting_widget_btn_close_clicked()
{
    emit _close();
}


void SettingWidget::on_language_activated(int index)
{

}

