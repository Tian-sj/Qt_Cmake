#pragma once

#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

template<typename... Args>
class FastSignal {
public:
    using slot_type     = std::function<void(Args...)>;
    using connection_id = size_t;

    class Connection {
    public:
        Connection() noexcept = default;
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
        ~Connection() { disconnect(); }
        void disconnect() {
            if (sig_) { sig_->disconnect(id_); sig_ = nullptr; id_ = 0; }
        }
        bool isConnected() const noexcept { return sig_ != nullptr; }
    private:
        FastSignal*   sig_{nullptr};
        connection_id id_{0};
    };

    FastSignal() : next_id_(1), slots_ptr_(std::make_shared<SlotList>()) {}
    ~FastSignal() = default;
    FastSignal(const FastSignal&) = delete;
    FastSignal& operator=(const FastSignal&) = delete;

    Connection connect(slot_type slot) {
        auto old_list = std::atomic_load(&slots_ptr_);
        auto new_list = std::make_shared<SlotList>(*old_list);
        auto id = next_id_.fetch_add(1, std::memory_order_relaxed);
        new_list->emplace_back(id, std::move(slot));
        std::atomic_store(&slots_ptr_, new_list);
        return Connection(this, id);
    }

    void disconnect(connection_id id) {
        auto old_list = std::atomic_load(&slots_ptr_);
        auto new_list = std::make_shared<SlotList>();
        new_list->reserve(old_list->size());
        for (auto& e : *old_list)
            if (e.first != id)
                new_list->push_back(std::move(e));
        std::atomic_store(&slots_ptr_, new_list);
    }

    void emit(Args... args) const {
        auto list = std::atomic_load(&slots_ptr_);
        for (auto& e : *list)
            e.second(args...);
    }

private:
    using SlotEntry = std::pair<connection_id, slot_type>;
    using SlotList  = std::vector<SlotEntry>;

    std::atomic<connection_id>           next_id_;
    std::shared_ptr<SlotList>            slots_ptr_;
};


class PrecisionCountdown {
public:
    using Clock = std::chrono::steady_clock;

    explicit PrecisionCountdown(int interval_ms = 10)
        : interval_(std::chrono::milliseconds(interval_ms))
        , thread_running_(true)
    {
        worker_ = std::thread([this]{ threadLoop(); });
    }

    ~PrecisionCountdown() {
        {
            std::lock_guard<std::mutex> lk(mtx_);
            thread_running_ = false;
            cv_.notify_all();
        }
        if (worker_.joinable())
            worker_.join();
    }

    // 启动或重启倒计时
    void start(int duration_ms) {
        auto now = Clock::now();
        std::lock_guard<std::mutex> lk(mtx_);
        end_time_   = now + std::chrono::milliseconds(std::max(duration_ms, 0));
        next_tick_  = now + interval_;
        remaining_  = std::max(duration_ms, 0);
        active_     = (duration_ms > 0);
        paused_     = false;
        cv_.notify_all();
    }

    // 停止
    void stop() {
        std::lock_guard<std::mutex> lk(mtx_);
        active_ = false;
        cv_.notify_all();
    }

    // 暂停：记录剩余时间
    void pause() {
        std::lock_guard<std::mutex> lk(mtx_);
        if (active_ && !paused_) {
            remaining_ = std::chrono::duration_cast<std::chrono::milliseconds>(
                             end_time_ - Clock::now()).count();
            paused_ = true;
        }
    }

    // 恢复：重设 end_time_、next_tick_
    void resume() {
        auto now = Clock::now();
        std::lock_guard<std::mutex> lk(mtx_);
        if (active_ && paused_) {
            end_time_  = now + std::chrono::milliseconds(remaining_);
            next_tick_ = now + interval_;
            paused_    = false;
            cv_.notify_all();
        }
    }

    bool isRunning() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return active_ && !paused_;
    }
    bool isPaused() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return paused_;
    }

    FastSignal<int> timeRemainingChanged;
    FastSignal<>    finished;

private:
    void threadLoop() {
        std::unique_lock<std::mutex> lk(mtx_);
        while (thread_running_) {
            // 等待启动
            cv_.wait(lk, [this]{ return !thread_running_ || (active_ && !paused_); });
            if (!thread_running_) break;

            // 主循环：直到结束或暂停、停止
            while (active_ && !paused_) {
                // 等待到下一个刻度或被打断
                cv_.wait_until(lk, next_tick_,
                               [this]{ return !thread_running_ || paused_ || !active_; });
                if (!thread_running_ || paused_ || !active_) break;

                auto now = Clock::now();
                if (now >= end_time_) {
                    active_ = false;
                    lk.unlock();
                    timeRemainingChanged.emit(0);
                    finished.emit();
                    lk.lock();
                    break;
                } else {
                    remaining_ = std::chrono::duration_cast<std::chrono::milliseconds>(
                                     end_time_ - now).count();
                    // 准备下一次刻度
                    next_tick_ += interval_;
                    lk.unlock();
                    timeRemainingChanged.emit(static_cast<int>(remaining_));
                    lk.lock();
                }
            }
        }
    }

    mutable std::mutex               mtx_;
    std::condition_variable          cv_;
    std::thread                      worker_;
    bool                             thread_running_;
    bool                             active_{false};
    bool                             paused_{false};
    std::chrono::milliseconds        interval_;
    Clock::time_point                end_time_;
    Clock::time_point                next_tick_;
    long long                        remaining_{0};
};
