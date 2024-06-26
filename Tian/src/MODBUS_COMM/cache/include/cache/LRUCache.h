/*
 * @Author: Tian_sj
 * @Date: 2023-10-30 14:44:55
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-11-01 16:13:49
 * @FilePath: /communication/cache/include/cache/LRUCache.h
 * @Description:u
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <map>
#include <unordered_map>
#include <list>
#include <iostream>
#include <mutex>
#include <optional>
#include <vector>

template <typename Key, typename Value, bool UseOrdered = false>
class LRUCache
{
private:
    size_t capacity;
    std::mutex mtx_;

    using CacheContainer = std::conditional_t<UseOrdered, std::map<Key, typename std::list<std::pair<Key, Value>>::iterator>, std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator>>;
    CacheContainer cacheMap;

    std::list<std::pair<Key, Value>> cacheList;

public:
    LRUCache(size_t capacity) : capacity(capacity) {}

    void put(const Key &key, const Value &value)
    {
        std::unique_lock<std::mutex> lock(mtx_);

        auto it = cacheMap.find(key);
        if (it != cacheMap.end())
        {
            it->second->second = std::move(value);
            cacheList.splice(cacheList.begin(), cacheList, it->second);
            return;
        }

        if (cacheMap.size() >= capacity)
        {
            Key keyToRemove = cacheList.back().first;
            cacheList.pop_back();
            cacheMap.erase(keyToRemove);
        }
        cacheList.emplace_front(key, std::move(value));
        cacheMap[key] = cacheList.begin();
    }

    std::optional<Value> get(const Key &key)
    {
        std::unique_lock<std::mutex> lock(mtx_);

        auto it = cacheMap.find(key);
        if (it != cacheMap.end())
        {
            cacheList.splice(cacheList.begin(), cacheList, it->second);
            return it->second->second;
        }
        return std::nullopt;
    }

    std::vector<Value> get(const std::vector<Key> &keys)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        std::vector<Value> result;

        for (const auto &key : keys)
        {
            auto it = cacheMap.find(key);
            if (it != cacheMap.end())
            {
                cacheList.splice(cacheList.begin(), cacheList, it->second);
                result.push_back(it->second->second);
            }
            else
            {
                result.push_back(Value{});
            }
        }

        return result;
    }

    void display()
    {

        if (!UseOrdered)
        {
            std::unique_lock<std::mutex> lock(mtx_);
            for (const auto &pair : cacheList)
                std::cout << pair.first << " : " << pair.second << std::endl;
        }
        else
        {
            std::unique_lock<std::mutex> lock(mtx_);
            for (const auto &pair : cacheMap)
                std::cout << pair.first << " : " << pair.second->second << std::endl;
        }
    }

    Value &operator[](const Key &key)
    {
        std::unique_lock<std::mutex> lock(mtx_);

        auto it = cacheMap.find(key);
        if (it != cacheMap.end())
        {
            cacheList.splice(cacheList.begin(), cacheList, it->second);
            return it->second->second;
        }

        if (cacheMap.size() >= capacity)
        {
            Key keyToRemove = cacheList.back().first;
            cacheList.pop_back();
            cacheMap.erase(keyToRemove);
        }

        cacheList.emplace_front(key, Value{});
        cacheMap[key] = cacheList.begin();
        return cacheMap[key]->second;
    }
};

#endif // LRUCACHE_H