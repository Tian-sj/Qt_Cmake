#include "qtcpp/precision_countdown/precision_countdown.hpp"
#include "test_support.hpp"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace {

using namespace std::chrono_literals;
using qtcpp::precision_countdown::PrecisionCountdown;
using qtcpp::precision_countdown::Signal;

void signal_connections_disconnect_safely() {
    int calls = 0;
    Signal<int>::Connection connection;
    {
        Signal<int> signal;
        connection = signal.connect([&calls](const int value) { calls += value; });
        signal.publish(2);
        qtcpp::test::require_equal(calls, 2, "subscriber should be called");
        connection.disconnect();
        signal.publish(2);
        qtcpp::test::require_equal(calls, 2, "disconnected subscriber should not be called");
    }
    connection.disconnect();
}

void zero_duration_finishes() {
    PrecisionCountdown countdown{5ms};
    std::mutex mutex;
    std::condition_variable condition;
    bool finished = false;
    auto connection = countdown.finished().connect([&] {
        {
            const std::scoped_lock lock{mutex};
            finished = true;
        }
        condition.notify_one();
    });

    countdown.start(0ms);
    std::unique_lock lock{mutex};
    const auto completed = condition.wait_for(lock, 1s, [&] { return finished; });
    qtcpp::test::require(completed, "zero-duration timer should finish promptly");
}

void pause_and_resume_preserve_remaining_time() {
    PrecisionCountdown countdown{10ms};
    countdown.start(250ms);
    std::this_thread::sleep_for(40ms);
    countdown.pause();
    const auto paused_remaining = countdown.remaining();
    std::this_thread::sleep_for(50ms);
    const auto still_remaining = countdown.remaining();
    qtcpp::test::require(countdown.paused(), "timer should be paused");
    qtcpp::test::require((paused_remaining - still_remaining) < 5ms,
                         "remaining time should not advance while paused");

    countdown.resume();
    qtcpp::test::require(countdown.running(), "timer should resume");
}

} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
    qtcpp::test::Suite suite;
    suite.add("signal connection lifetime", signal_connections_disconnect_safely);
    suite.add("zero-duration countdown", zero_duration_finishes);
    suite.add("pause and resume", pause_and_resume_preserve_remaining_time);
    return suite.run();
}
