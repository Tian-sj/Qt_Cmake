#include "precision_countdown/precision_countdown.hpp"

#include <iostream>
#include <thread>

int main() {
    PrecisionCountdown timer(1);

    auto a = timer.timeRemainingChanged.connect([](int ms) {
        std::cout << "剩余 " << ms << " ms" << std::endl;
    });

    auto b = timer.finished.connect([](){
        std::cout << "倒计时结束!" << std::endl;
    });

    timer.start(3000);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
}
