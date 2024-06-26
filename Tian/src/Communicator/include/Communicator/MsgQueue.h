/*!
 * @file          MsgQueue.h
 * @brief         消息队列
 * @author        Tian_sj
 * @date          2024-03-04
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
    explicit MsgQueue(size_t capacity) : capacity(capacity)
    {
    }

    /*!
     * @brief         加入消息到消息队列
     * 
     * @param         data
     * @attention
     */
    void push(T data)
    {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            if (q_.size() >= capacity)
                q_.pop_back();
            q_.emplace_back(std::move(data));
        }
        cdv_.notify_one();
    }

    /*!
     * @brief         删除消息队列头部消息，并返回相应的数值
     * 
     * @return        T
     * @attention
     */
    T pop()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        cdv_.wait(lock, [this]
                  { return !q_.empty(); });
        auto ret = std::move(q_.front());
        q_.pop_front();
        return ret;
    }

    /*!
     * @brief         删除消息队列头部消息，返回是否删除成功,有参数版本
     * 
     * @param         data
     * @return        true
     * @return        false
     * @attention
     */
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

    /*!
     * @brief         删除消息队列头部消息，返回是否删除成功,无参数版本
     * 
     * @return        true
     * @return        false
     * @attention
     */
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

    /*!
     * @brief         返回消息队列头部消息到data中,并判断是否返回成功
     * 
     * @param         data
     * @return        true
     * @return        false
     * @attention
     */
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

    /*!
     * @brief         返回消息队列头部消息
     * 
     * @return        T
     * @attention
     */
    T front()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        return q_.front();
    }

    /*!
     * @brief         返回消息队列尾部消息到data中,并判断是否返回成功
     * 
     * @param         data
     * @return        true
     * @return        false
     * @attention
     */
    bool back(T &data)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (q_.empty())
        {
            return false;
        }
        data = q_.back();
        return true;
    }

    /*!
     * @brief         更新消息队列中的消息
     * 
     * @param         data
     * @attention
     */
    void update_back(const T &data) {
        std::unique_lock<std::mutex> lock(mtx_);
        if (!q_.empty())
            q_[q_.size() - 1] = data;
    }

    /*!
     * @brief         判断消息队列是否为空
     * 
     * @return        true
     * @return        false
     * @attention
     */
    bool empty()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        return q_.empty();
    }

    /*!
     * @brief         返回消息队列大小
     * 
     * @return        size_t
     * @attention
     */
    size_t size()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        return q_.size();
    }

    /*!
     * @brief         清空消息队列
     * 
     * @attention
     */
    void clear()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        q_.clear();
    }
};

#endif // MSGQUEUE_H
