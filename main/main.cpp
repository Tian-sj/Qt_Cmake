/*
 * @Author: Tian_sj
 * @Date: 2023-09-15 10:39:22
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-22 11:02:32
 * @FilePath: /Qt_Cmake/main/main.cpp
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#include <QApplication>

#include <app/mainwindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
