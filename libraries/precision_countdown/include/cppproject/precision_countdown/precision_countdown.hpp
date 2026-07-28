#pragma once

#include "cppproject/precision_countdown/export.hpp"
#include "cppproject/precision_countdown/signal.hpp"

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

namespace cppproject::precision_countdown {

class CPPPROJECT_PRECISION_COUNTDOWN_EXPORT PrecisionCountdown final {
public:
    using Duration = std::chrono::milliseconds;

    explicit PrecisionCountdown(Duration interval = Duration{10});
    PrecisionCountdown(const PrecisionCountdown&) = delete;
    PrecisionCountdown& operator=(const PrecisionCountdown&) = delete;
    PrecisionCountdown(PrecisionCountdown&&) = delete;
    PrecisionCountdown& operator=(PrecisionCountdown&&) = delete;
    ~PrecisionCountdown();

    void start(Duration duration);
    void stop();
    void pause();
    void resume();
    void set_interval(Duration interval);

    [[nodiscard]] bool running() const;
    [[nodiscard]] bool paused() const;
    [[nodiscard]] Duration remaining() const;

    // 回调在内部工作线程执行；Qt 界面层应使用 queued connection 切回 GUI 线程。
    Signal<Duration>& time_remaining_changed() noexcept {
        return time_remaining_changed_;
    }
    Signal<>& finished() noexcept {
        return finished_;
    }

private:
    using Clock = std::chrono::steady_clock;

    void run();
    static Duration positive_interval(Duration interval);

    mutable std::mutex mutex_;
    std::condition_variable condition_;
    bool stopping_{false};
    bool active_{false};
    bool paused_{false};
    std::uint64_t generation_{0};
    Duration interval_;
    Duration remaining_{0};
    Clock::time_point end_time_{};
    Signal<Duration> time_remaining_changed_;
    Signal<> finished_;
    std::thread worker_;
};

} // namespace cppproject::precision_countdown
