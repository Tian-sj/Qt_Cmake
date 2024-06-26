/*
 * @Author: Tian_sj
 * @Date: 2023-09-22 19:42:36
 * @LastEditors: Tian_sj tsj3911@163.com
 * @LastEditTime: 2023-10-31 17:50:15
 * @FilePath: /communication/comm/include/comm/MsgQueue.h
 * @Description:
 * Copyright (c) 2023 by 传麒科技(北京)股份有限公司.
 */
#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <mutex>
#include <condition_variable>
#include <deque>

template <typename T>
class MsgQueue
{
private:
    size_t capacity;
    std::deque<T> q_;
    std::mutex mtx_;
    std::condition_variable cdv_;

public:
    MsgQueue(size_t capacity) : capacity(capacity)
    {
    }

    void push(T &&data)
    {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            if (q_.size() >= capacity)
                q_.pop_back();
            q_.emplace_back(std::move(data));
        }
        cdv_.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        cdv_.wait(lock, [this]
                  { return !q_.empty(); });
        auto ret = std::move(q_.front());
        q_.pop_front();
        return ret;
    }

    bool try_pop(T &data)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (q_.empty())
        {
            return false;
        }
        data = std::move(q_.front());
        q_.pop_front();
        return true;
    }

    bool try_pop()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (q_.empty())
        {
            return false;
        }
        q_.pop_front();
        return true;
    }

    // 获取队列前端值（不移除）
    bool front(T &data)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (q_.empty())
        {
            return false;
        }
        data = q_.front();
        return true;
    }

    bool empty()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        return q_.empty();
    }

    size_t size()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        return q_.size();
    }

    void clear()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        q_.clear();
    }
};

#endif // MSGQUEUE_H
