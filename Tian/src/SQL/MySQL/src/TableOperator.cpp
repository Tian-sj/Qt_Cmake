#include <Tian/SQL/MySQL/TableOperator.h>

TableOperator::TableOperator(QObject *parent) : MySQLHandler(parent)
{
    // 初始化操作，如果需要的话
}

bool TableOperator::createTable(const QString &tableName, const QStringList &fields)
{
    QString queryStr = "CREATE TABLE " + tableName + " (";
    for (const QString &field : fields)
    {
        queryStr += field + ", ";
    }
    queryStr = queryStr.left(queryStr.length() - 2); // 删除最后的逗号和空格
    queryStr += ")";

    return executeQuery([this, queryStr]()
                        {
        QSqlQuery query(m_db);
        return query.exec(queryStr); });
}

bool TableOperator::insertData(const QString &tableName, const QMap<QString, QVariant> &data)
{
    QString fieldNames = data.keys().join(", ");
    QStringList placeHolders;
    for (int i = 0; i < data.size(); ++i)
    {
        placeHolders << "?";
    }
    QString queryStr = "INSERT INTO " + tableName + " (" + fieldNames + ") VALUES (" + placeHolders.join(", ") + ")";

    return executeQuery([this, queryStr, data]()
                        {
        QSqlQuery query(m_db);
        query.prepare(queryStr);

        int index = 0;
        for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
            query.bindValue(index, it.value());
            ++index;
        }

        return query.exec(); });
}

bool TableOperator::updateData(const QString &tableName, const QString &condition, const QMap<QString, QVariant> &data)
{
    QStringList setFields;
    for (auto it = data.constBegin(); it != data.constEnd(); ++it)
    {
        setFields << it.key() + " = :" + it.key();
    }

    QString queryStr = "UPDATE " + tableName + " SET " + setFields.join(", ") + " WHERE " + condition;

    return executeQuery([this, queryStr, data]()
                        {
        QSqlQuery query(m_db);
        query.prepare(queryStr);

        for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
            query.bindValue(":" + it.key(), it.value());
        }

        return query.exec(); });
}

bool TableOperator::deleteData(const QString &tableName, const QString &condition)
{
    QString queryStr = "DELETE FROM " + tableName + " WHERE " + condition;

    return executeQuery([this, queryStr]()
                        {
        QSqlQuery query(m_db);
        return query.exec(queryStr); });
}

QList<QMap<QString, QVariant>> TableOperator::selectData(const QString &tableName, const QString &condition)
{
    QString queryStr = "SELECT * FROM " + tableName + " WHERE " + condition;

    QList<QMap<QString, QVariant>> results;

    bool success = executeQuery([this, queryStr, &results]()
                                {
        QSqlQuery query(m_db);
        if(query.exec(queryStr)) {
            while(query.next()) {
                QMap<QString, QVariant> rowData;
                QSqlRecord record = query.record();
                for(int i = 0; i < record.count(); ++i) {
                    rowData[record.fieldName(i)] = query.value(i);
                }
                results.append(rowData);
            }
            return true;
        }
        return false; });

    if (!success)
    {
        // TODO 处理错误或返回空结果
    }

    return results;
}
QList<QMap<QString, QVariant>> TableOperator::selectData(const QString &tableName)
{
    QString queryStr = "SELECT * FROM " + tableName;

    QList<QMap<QString, QVariant>> results;

    bool success = executeQuery([this, queryStr, &results]()
                                {
        QSqlQuery query(m_db);
        if(query.exec(queryStr)) {
            while(query.next()) {
                QMap<QString, QVariant> rowData;
                QSqlRecord record = query.record();
                for(int i = 0; i < record.count(); ++i) {
                    rowData[record.fieldName(i)] = query.value(i);
                }
                results.append(rowData);
            }
            return true;
        }
        return false; });

    if (!success)
    {
        // TODO 处理错误或返回空结果
    }

    return results;
}

void TableOperator::selectDataModel(QSqlQueryModel *const model, const QString &tableName, const QString &condition)
{
    QString queryStr = "SELECT * FROM " + tableName + " WHERE " + condition;

    model->setQuery(queryStr, m_db); // 或者使用 setQuery(queryStr, db) 传递数据库连接

    if (model->lastError().isValid())
    {
        // 处理错误
        qDebug() << model->lastError();
    }
}

void TableOperator::selectDataModel(QSqlQueryModel *const model, const QString &tableName)
{
    QString queryStr = "SELECT * FROM " + tableName;

    model->setQuery(queryStr, m_db); // 或者使用 setQuery(queryStr, db) 传递数据库连接

    if (model->lastError().isValid())
    {
        // 处理错误
        qDebug() << model->lastError();
    }
}