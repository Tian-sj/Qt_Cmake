/*
 * @Author: Tian_sj
 * @Date: 2023-11-02 11:38:17
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-02 14:37:52
 * @FilePath: /MySQL/src/MySQLHandler.cpp
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */

#include <Tian/SQL/MySQL/MySQLHandler.h>

MySQLHandler::MySQLHandler(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}

void MySQLHandler::connectToDatabase(const QString &host, const QString &user, const QString &password, const QString &databaseName, int port)
{
    if (m_db.isOpen())
    {
        m_db.close();
    }

    m_db.setHostName(host);
    m_db.setUserName(user);
    m_db.setPassword(password);
    m_db.setDatabaseName(databaseName);
    m_db.setPort(port);

    if (m_db.open())
    {
        qDebug() << "Connected to database";
    }
    else
    {
        logError("Failed to connect to database: " + m_db.lastError().text());
    }
}

void MySQLHandler::disconnectFromDatabase()
{
    m_db.close();
    qDebug() << "Disconnected from database";
}
