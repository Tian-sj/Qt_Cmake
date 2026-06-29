#include "qtcpp/precision_countdown/precision_countdown.hpp"

#include <algorithm>
#include <chrono>

namespace qtcpp::precision_countdown {

PrecisionCountdown::PrecisionCountdown(const Duration interval)
    : interval_{positive_interval(interval)}, worker_{[this] { run(); }} {}

PrecisionCountdown::~PrecisionCountdown() {
    {
        const std::scoped_lock lock{mutex_};
        stopping_ = true;
        ++generation_;
    }
    condition_.notify_all();
    if (worker_.joinable()) {
        worker_.join();
    }
}

void PrecisionCountdown::start(const Duration duration) {
    {
        const std::scoped_lock lock{mutex_};
        remaining_ = std::max(duration, Duration{0});
        end_time_ = Clock::now() + remaining_;
        active_ = true;
        paused_ = false;
        ++generation_;
    }
    condition_.notify_all();
}

void PrecisionCountdown::stop() {
    {
        const std::scoped_lock lock{mutex_};
        active_ = false;
        paused_ = false;
        remaining_ = Duration{0};
        ++generation_;
    }
    condition_.notify_all();
}

void PrecisionCountdown::pause() {
    {
        const std::scoped_lock lock{mutex_};
        if (!active_ || paused_) {
            return;
        }
        remaining_ =
            std::max(std::chrono::duration_cast<Duration>(end_time_ - Clock::now()), Duration{0});
        paused_ = true;
        ++generation_;
    }
    condition_.notify_all();
}

void PrecisionCountdown::resume() {
    {
        const std::scoped_lock lock{mutex_};
        if (!active_ || !paused_) {
            return;
        }
        end_time_ = Clock::now() + remaining_;
        paused_ = false;
        ++generation_;
    }
    condition_.notify_all();
}

void PrecisionCountdown::set_interval(const Duration interval) {
    {
        const std::scoped_lock lock{mutex_};
        interval_ = positive_interval(interval);
        ++generation_;
    }
    condition_.notify_all();
}

bool PrecisionCountdown::running() const {
    const std::scoped_lock lock{mutex_};
    return active_ && !paused_;
}

bool PrecisionCountdown::paused() const {
    const std::scoped_lock lock{mutex_};
    return active_ && paused_;
}

PrecisionCountdown::Duration PrecisionCountdown::remaining() const {
    const std::scoped_lock lock{mutex_};
    if (active_ && !paused_) {
        return std::max(std::chrono::duration_cast<Duration>(end_time_ - Clock::now()),
                        Duration{0});
    }
    return remaining_;
}

void PrecisionCountdown::run() {
    std::unique_lock lock{mutex_};
    while (!stopping_) {
        condition_.wait(lock, [this] { return stopping_ || (active_ && !paused_); });
        if (stopping_) {
            break;
        }

        const auto now = Clock::now();
        if (now >= end_time_) {
            active_ = false;
            remaining_ = Duration{0};
            lock.unlock();
            time_remaining_changed_.publish(Duration{0});
            finished_.publish();
            lock.lock();
            continue;
        }

        const auto observed_generation = generation_;
        const auto wake_time = std::min(end_time_, now + interval_);
        const bool interrupted =
            condition_.wait_until(lock, wake_time, [this, observed_generation] {
                return stopping_ || !active_ || paused_ || generation_ != observed_generation;
            });
        if (interrupted) {
            continue;
        }

        const auto after_wait = Clock::now();
        if (after_wait >= end_time_) {
            active_ = false;
            remaining_ = Duration{0};
            lock.unlock();
            time_remaining_changed_.publish(Duration{0});
            finished_.publish();
            lock.lock();
            continue;
        }

        remaining_ = std::chrono::duration_cast<Duration>(end_time_ - after_wait);
        const auto value = remaining_;
        lock.unlock();
        time_remaining_changed_.publish(value);
        lock.lock();
    }
}

PrecisionCountdown::Duration PrecisionCountdown::positive_interval(const Duration interval) {
    return std::max(interval, Duration{1});
}

} // namespace qtcpp::precision_countdown
