/*
 * @Author: Tian_sj
 * @Date: 2023-10-16 10:48:59
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-07 12:57:58
 * @FilePath: /communication/cache/src/cachebase.cpp
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#include <cache/cachebase.h>
#include <iostream>

CacheBase::CacheBase(QObject *parent)
    : QObject{parent},
      m_data(10)
{
}

void CacheBase::registerCallback(uint16_t address, TextCallback callback)
{
    m_update_view.insert(std::pair<uint16_t, TextCallback>(address, callback));
}

void CacheBase::update_model(uint16_t address, const uint16_t &sum, const QList<uint16_t> &receive_buffer)
{
    for (uint16_t i = 0; i < sum; ++i, ++address)
    {
        m_data.put(address, static_cast<int>(receive_buffer[i]));
        try
        {
            auto range = m_update_view.equal_range(address);
            for (auto it = range.first; it != range.second; ++it)
            {
                if (it->second)
                    it->second(m_data.get(it->first).value());
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << ":"
                      << "address" << address << '\n';
        }
    }
}

void CacheBase::cleaup_view()
{
    for (auto it = m_update_view.cbegin(); it != m_update_view.cend(); ++it)
    {
        it->second(0);
        m_data.put(it->first, 0);
    }
}

std::vector<int> CacheBase::get_cache(uint16_t start_address, uint16_t num)
{
    std::vector<uint16_t> keys;
    while (num--)
        keys.push_back(start_address++);
    return m_data.get(keys);
}
