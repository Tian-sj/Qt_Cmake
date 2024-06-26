/*
 * @Author: Tian_sj
 * @Date: 2023-11-02 11:36:37
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-02 12:02:09
 * @FilePath: /MySQL/include/MySQL/MySQLHandler.h
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#ifndef MYSQLHANDLER_H
#define MYSQLHANDLER_H

#include <QObject>
#include <QtSql>
#include <QtConcurrent>
#include <QFuture>
#include <functional>
#include <QDebug>

class MySQLHandler : public QObject
{
    Q_OBJECT

public:
    explicit MySQLHandler(QObject *parent = nullptr);

    void connectToDatabase(const QString &host, const QString &user, const QString &password, const QString &databaseName, int port = 3306);
    void disconnectFromDatabase();

    template <typename Func>
    bool executeQuery(Func &&func)
    {
        if (m_db.isOpen())
        {
            auto future = QtConcurrent::run([this, func]()
                                            { func(); });
            future.waitForFinished(); // Wait for the execution to finish
            return true;              // Return true upon successful completion
        }
        else
        {
            qDebug() << "Database is not open";
            return false; // Return false if the database is not open
        }
    }

protected:
    QSqlDatabase m_db;

    void logError(const QString &error)
    {
        // 这里可以添加错误日志记录到文件或者其他记录器
        qDebug() << "Error:" << error;
    }
};

#endif // MYSQLHANDLER_H
