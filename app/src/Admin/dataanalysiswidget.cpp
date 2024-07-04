#include <app/Admin/dataanalysiswidget.h>

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
}

void DataAnalysisWidget::initUIText()
{

}

void DataAnalysisWidget::initConnect()
{

}

}
