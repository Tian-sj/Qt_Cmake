/*
 * @Author: Tian_sj
 * @Date: 2023-09-15 10:39:22
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-10-16 12:14:42
 * @FilePath: /BTS/app/src/main.cpp
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <app/mainwindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "D2023022806_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName))
        {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
