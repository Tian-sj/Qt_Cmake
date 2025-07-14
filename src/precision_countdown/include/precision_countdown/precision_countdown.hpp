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
            if (sig_) {
                sig_->disconnect(id_);
                sig_ = nullptr;
                id_ = 0;
            }
        }
        bool isConnected() const noexcept { return sig_ != nullptr; }

    private:
        FastSignal*   sig_{nullptr};
        connection_id id_{0};
    };

    FastSignal()
        : next_id_(1)
        , slots_ptr_(std::make_shared<SlotList>())
    {}

    ~FastSignal() = default;
    FastSignal(const FastSignal&) = delete;
    FastSignal& operator=(const FastSignal&) = delete;

    // Connect a new slot. Thread-safe.
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

    // Disconnect slot by id. Thread-safe.
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

    // Trigger all slots with given arguments. Exceptions in one slot won't stop others.
    void trigger(Args... args) const {
        auto list = std::atomic_load_explicit(&slots_ptr_, std::memory_order_acquire);
        for (auto& e : *list) {
            try {
                e.second(args...);
            } catch (...) {
                // swallow or log
            }
        }
    }

    // Query number of connected slots.
    size_t size() const noexcept {
        auto list = std::atomic_load_explicit(&slots_ptr_, std::memory_order_acquire);
        return list->size();
    }
    bool empty() const noexcept { return size() == 0; }

private:
    using SlotEntry = std::pair<connection_id, slot_type>;
    using SlotList  = std::vector<SlotEntry>;

    std::atomic<connection_id>    next_id_;
    std::shared_ptr<SlotList>     slots_ptr_;
};

// ========================
// PrecisionCountdown
// ========================
class PrecisionCountdown {
public:
    using Clock = std::chrono::steady_clock;

    explicit PrecisionCountdown(int interval_ms = 10)
        : interval_(std::chrono::milliseconds(interval_ms))
        , thread_running_(true)
        , active_(false)
        , paused_(false)
    {
        worker_ = std::thread([this]{ threadLoop(); });
    }

    ~PrecisionCountdown() {
        // signal thread to exit
        thread_running_.store(false, std::memory_order_release);
        cv_.notify_all();
        if (worker_.joinable()) {
            worker_.join();
        }
    }

    // Start or restart countdown
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

    // Stop countdown immediately
    void stop() {
        active_.store(false, std::memory_order_release);
        cv_.notify_all();
    }

    // Pause countdown, preserving remaining time
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

    // Resume countdown from paused state
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

    // Change tick interval (ms); takes effect on next cycle
    void setInterval(int interval_ms) {
        std::lock_guard<std::mutex> lk(mtx_);
        interval_ = std::chrono::milliseconds(std::max(interval_ms, 1));
        cv_.notify_all();
    }

    bool isRunning() const noexcept {
        return active_.load(std::memory_order_acquire)
        && !paused_.load(std::memory_order_acquire);
    }

    bool isPaused() const noexcept {
        return paused_.load(std::memory_order_acquire);
    }

    FastSignal<int> timeRemainingChanged;  // in milliseconds
    FastSignal<>    finished;

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
    mutable std::mutex               mtx_;
    std::condition_variable          cv_;
    std::thread                      worker_;

    std::atomic<bool>                thread_running_;
    std::atomic<bool>                active_;
    std::atomic<bool>                paused_;

    std::chrono::milliseconds        interval_;
    Clock::time_point                end_time_;
    long long                        remaining_{0};
};

} // namespace timer
