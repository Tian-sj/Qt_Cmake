/*
 * @Author: Tian_sj
 * @Date: 2023-11-02 11:58:08
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-02 14:37:35
 * @FilePath: /MySQL/include/Tian/SQL/MySQL/TableOperator.h
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#ifndef TABLEOPERATOR_H
#define TABLEOPERATOR_H

#include <Tian/SQL/MySQL/MySQLHandler.h>

class TableOperator : public MySQLHandler
{
public:
    TableOperator(QObject *parent = nullptr);

    bool createTable(const QString &tableName, const QStringList &fields);
    bool insertData(const QString &tableName, const QMap<QString, QVariant> &data);
    bool updateData(const QString &tableName, const QString &condition, const QMap<QString, QVariant> &data);
    bool deleteData(const QString &tableName, const QString &condition);
    QList<QMap<QString, QVariant>> selectData(const QString &tableName, const QString &condition);
    QList<QMap<QString, QVariant>> selectData(const QString &tableName);
    void selectDataModel(QSqlQueryModel *const model, const QString &tableName, const QString &condition);
    void selectDataModel(QSqlQueryModel *const model, const QString &tableName);
};

#endif // TABLEOPERATOR_H
