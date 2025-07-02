#include "gui/mainobject.hpp"
#include "version.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>

MainObject::MainObject()
{
}

MainObject::~MainObject()
{
}

void MainObject::invoke_init_ui_text()
{
    init_ui_text();
}

QString MainObject::stdtm_to_QString(std::tm tm_value)
{
    time_t time_value = mktime(&tm_value);  // 将 std::tm 转换为 time_t

    // Qt 5 中使用 fromTime_t，Qt 6 中请使用 fromSecsSinceEpoch
    QDateTime date_time = QDateTime::fromSecsSinceEpoch(time_value);

    return date_time.toString("yyyy-MM-dd hh:mm:ss");
}

void MainObject::loadCSS(QWidget* parrent, const QString &cssFile)
{
    QFile file(cssFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    QTextStream stream(&file);
    QString style = stream.readAll();

    parrent->setStyleSheet(style);

    file.close();
}
