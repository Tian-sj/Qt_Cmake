#pragma once

#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <memory>
#include <algorithm>

namespace timer {

/*!
 * @brief         Lightweight thread-safe signal/slot mechanism
 *
 * 轻量级线程安全信号槽机制
 *
 * @tparam        Args...   Slot 参数类型
 *
 * @attention     Allows multiple slots to be connected, disconnected, and triggered in a lock-free manner.
 *                支持多个槽函数连接、断开并触发，且采用无锁结构实现。
 */
template<typename... Args>
class FastSignal {
public:
    using slot_type = std::function<void(Args...)>;
    using connection_id = size_t;

    /*!
     * @brief         连接句柄类
     *
     * 表示信号连接关系的管理器
     */
    class Connection {
    public:
        Connection() noexcept = default;

        /*!
         * @brief         构造连接对象
         *
         * @param         sig   所属信号对象指针
         * @param         id    槽函数唯一 ID
         */
        Connection(FastSignal* sig, connection_id id) noexcept
            : sig_(sig), id_(id) {}

        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        Connection(Connection&& o) noexcept { *this = std::move(o); }

        Connection& operator=(Connection&& o) noexcept {
            if (this != &o) {
                disconnect();
                sig_ = o.sig_; id_ = o.id_;
                o.sig_ = nullptr; o.id_ = 0;
            }
            return *this;
        }

        /*!
         * @brief         析构时断开连接
         */
        ~Connection() { disconnect(); }

        /*!
         * @brief         主动断开连接
         */
        void disconnect() {
            if (sig_) {
                sig_->disconnect(id_);
                sig_ = nullptr;
                id_ = 0;
            }
        }

        /*!
         * @brief         判断是否已连接
         *
         * @return        true 已连接，false 已断开
         */
        bool isConnected() const noexcept { return sig_ != nullptr; }

    private:
        FastSignal* sig_{nullptr};  //!< 所属信号对象
        connection_id id_{0};       //!< 槽函数 ID
    };

    /*!
     * @brief         构造函数
     */
    FastSignal()
        : next_id_(1)
        , slots_ptr_(std::make_shared<SlotList>())
    {}

    ~FastSignal() = default;
    FastSignal(const FastSignal&) = delete;
    FastSignal& operator=(const FastSignal&) = delete;

    /*!
     * @brief         连接槽函数
     *
     * 线程安全
     *
     * @param         slot   待连接的槽函数
     * @return        返回连接对象，可用于管理连接状态
     */
    Connection connect(slot_type slot) {
        auto old_list = std::atomic_load_explicit(&slots_ptr_, std::memory_order_acquire);
        auto new_list = std::make_shared<SlotList>();
        new_list->reserve(old_list->size() + 1);
        new_list->insert(new_list->end(), old_list->begin(), old_list->end());

        auto id = next_id_.fetch_add(1, std::memory_order_relaxed);
        new_list->emplace_back(id, std::move(slot));

        std::atomic_store_explicit(&slots_ptr_, new_list, std::memory_order_release);
        return Connection(this, id);
    }

    /*!
     * @brief         断开某一槽函数
     *
     * 线程安全
     *
     * @param         id   槽函数 ID
     */
    void disconnect(connection_id id) {
        auto old_list = std::atomic_load_explicit(&slots_ptr_, std::memory_order_acquire);
        auto new_list = std::make_shared<SlotList>();
        new_list->reserve(old_list->size());
        for (auto& e : *old_list) {
            if (e.first != id) {
                new_list->push_back(e);
            }
        }
        std::atomic_store_explicit(&slots_ptr_, new_list, std::memory_order_release);
    }

    /*!
     * @brief         触发所有槽函数
     *
     * @param         args... 传递给槽函数的参数
     */
    void trigger(Args... args) const {
        auto list = std::atomic_load_explicit(&slots_ptr_, std::memory_order_acquire);
        for (auto& e : *list) {
            try {
                e.second(args...);
            } catch (...) {
                // 异常吞噬或日志处理
            }
        }
    }

    /*!
     * @brief         获取当前连接的槽数量
     *
     * @return        槽数量
     */
    size_t size() const noexcept {
        auto list = std::atomic_load_explicit(&slots_ptr_, std::memory_order_acquire);
        return list->size();
    }

    /*!
     * @brief         判断是否没有槽连接
     *
     * @return        若无连接返回 true
     */
    bool empty() const noexcept { return size() == 0; }

private:
    using SlotEntry = std::pair<connection_id, slot_type>;
    using SlotList = std::vector<SlotEntry>;

    std::atomic<connection_id> next_id_;    //!< 槽函数唯一 ID 计数器
    std::shared_ptr<SlotList> slots_ptr_;   //!< 槽函数列表（共享指针实现无锁更新）
};

/*!
 * @brief         高精度倒计时器
 *
 * 精确定时器，支持暂停、恢复与间隔调整
 *
 * @attention     Backed by dedicated thread with real-time tracking.
 *                使用独立线程精准跟踪剩余时间。
 */
class PrecisionCountdown {
public:
    using Clock = std::chrono::steady_clock;

    /*!
     * @brief         构造函数
     *
     * @param         interval_ms   回调间隔，单位毫秒
     */
    explicit PrecisionCountdown(int interval_ms = 10)
        : interval_(std::chrono::milliseconds(interval_ms))
        , thread_running_(true)
        , active_(false)
        , paused_(false)
    {
        worker_ = std::thread([this]{ threadLoop(); });
    }

    /*!
     * @brief         析构函数（自动停止线程）
     */
    ~PrecisionCountdown() {
        // signal thread to exit
        thread_running_.store(false, std::memory_order_release);
        cv_.notify_all();
        if (worker_.joinable()) {
            worker_.join();
        }
    }

    /*!
     * @brief         启动倒计时
     *
     * @param         duration_ms   倒计时总时长（毫秒）
     */
    void start(int duration_ms) {
        auto now = Clock::now();
        std::lock_guard<std::mutex> lk(mtx_);
        remaining_  = std::max(duration_ms, 0);
        end_time_   = now + std::chrono::milliseconds(remaining_);
        interval_   = std::max(interval_, std::chrono::milliseconds(1));
        active_.store(remaining_ > 0, std::memory_order_release);
        paused_.store(false, std::memory_order_release);
        cv_.notify_all();
    }

    /*!
     * @brief         立即停止倒计时
     */
    void stop() {
        active_.store(false, std::memory_order_release);
        cv_.notify_all();
    }

    /*!
     * @brief         暂停倒计时（保留剩余时间）
     */
    void pause() {
        if (active_.load(std::memory_order_acquire)
            && !paused_.load(std::memory_order_acquire)) {
            auto now = Clock::now();
            std::lock_guard<std::mutex> lk(mtx_);
            remaining_ = std::chrono::duration_cast<std::chrono::milliseconds>(
                             end_time_ - now).count();
            paused_.store(true, std::memory_order_release);
        }
    }

    /*!
     * @brief         恢复倒计时（从暂停位置继续）
     */
    void resume() {
        if (active_.load(std::memory_order_acquire)
            && paused_.load(std::memory_order_acquire)) {
            auto now = Clock::now();
            std::lock_guard<std::mutex> lk(mtx_);
            end_time_ = now + std::chrono::milliseconds(remaining_);
            paused_.store(false, std::memory_order_release);
            cv_.notify_all();
        }
    }

    /*!
     * @brief         设置触发间隔（下次周期起生效）
     *
     * @param         interval_ms   间隔时间（毫秒）
     */
    void setInterval(int interval_ms) {
        std::lock_guard<std::mutex> lk(mtx_);
        interval_ = std::chrono::milliseconds(std::max(interval_ms, 1));
        cv_.notify_all();
    }

    /*!
     * @brief         判断是否正在运行
     *
     * @return        true 表示倒计时正在进行且未暂停
     */
    bool isRunning() const noexcept {
        return active_.load(std::memory_order_acquire)
        && !paused_.load(std::memory_order_acquire);
    }

    /*!
     * @brief         判断是否处于暂停状态
     *
     * @return        true 表示倒计时已暂停
     */
    bool isPaused() const noexcept {
        return paused_.load(std::memory_order_acquire);
    }

    FastSignal<int> timeRemainingChanged;   //!< 剩余时间更新信号（单位：毫秒）
    FastSignal<> finished;                  //!< 倒计时完成信号

private:
    void threadLoop() {
        std::unique_lock<std::mutex> lk(mtx_);
        while (thread_running_.load(std::memory_order_acquire)) {
            // wait until started or thread termination
            cv_.wait(lk, [&]{
                return !thread_running_.load(std::memory_order_acquire)
                || (active_.load(std::memory_order_acquire)
                    && !paused_.load(std::memory_order_acquire));
            });
            if (!thread_running_.load(std::memory_order_acquire))
                break;

            // countdown loop
            while (active_.load(std::memory_order_acquire)
                   && !paused_.load(std::memory_order_acquire))
            {
                auto now = Clock::now();
                if (now >= end_time_) {
                    // finished
                    active_.store(false, std::memory_order_release);
                    lk.unlock();
                    timeRemainingChanged.trigger(0);
                    finished.trigger();
                    lk.lock();
                    break;
                }

                // compute time until next tick
                auto ms_left   = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ - now).count();
                auto wait_ms   = std::min<long long>(interval_.count(), ms_left);
                // wait or be interrupted
                cv_.wait_for(lk, std::chrono::milliseconds(wait_ms), [&]{
                    return !thread_running_.load(std::memory_order_acquire)
                    || paused_.load(std::memory_order_acquire)
                        || !active_.load(std::memory_order_acquire);
                });
                if (!thread_running_.load(std::memory_order_acquire)
                    || paused_.load(std::memory_order_acquire)
                    || !active_.load(std::memory_order_acquire))
                {
                    break;
                }

                // update remaining and emit
                remaining_ = ms_left - wait_ms;
                lk.unlock();
                timeRemainingChanged.trigger(static_cast<int>(remaining_));
                lk.lock();
            }
        }
    }

private:
    mutable std::mutex mtx_;                    //!< 互斥锁
    std::condition_variable cv_;                //!< 条件变量
    std::thread worker_;                        //!< 工作线程

    std::atomic<bool> thread_running_;          //!< 线程是否运行
    std::atomic<bool> active_;                  //!< 倒计时是否激活
    std::atomic<bool> paused_;                  //!< 是否处于暂停

    std::chrono::milliseconds interval_;        //!< 回调间隔
    Clock::time_point end_time_;                //!< 预计结束时间
    long long remaining_{0};                    //!< 剩余时间（毫秒）
};

} // namespace timer
