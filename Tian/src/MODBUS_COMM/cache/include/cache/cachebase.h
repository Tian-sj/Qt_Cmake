/*
 * @Author: Tian_sj
 * @Date: 2023-10-16 10:48:59
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-01 16:03:48
 * @FilePath: /communication/cache/include/cache/cachebase.h
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#ifndef CACHEBASE_H
#define CACHEBASE_H

#include <QObject>
#include <functional>
#include <map>
#include <unordered_map>
#include <QByteArray>
#include <cache/LRUCache.h>
#include <vector>

class CacheBase : public QObject
{
    Q_OBJECT
public:
    using TextCallback = std::function<void(const int &)>;

    explicit CacheBase(QObject *parent = nullptr);

    void registerCallback(uint16_t address, TextCallback callback);

    void update_model(uint16_t address, const uint16_t &sum, const QList<uint16_t> &receive_buffer);

    void cleaup_view();

    std::vector<int> get_cache(uint16_t start_address, uint16_t num);

protected:
    std::unordered_multimap<uint16_t, TextCallback> m_update_view;

private:
    LRUCache<uint16_t, int> m_data;
};

#endif // CACHEBASE_H
