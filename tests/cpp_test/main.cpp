/*
#include <iostream>
#include <string>
#include <ranges>
#include <sstream>

int main(){
    // std::string s = "apple,banana,cherry";
    // C++20
    for (auto &&subrange : s | std::views::split(',')) {
        std::string part(subrange.begin(), subrange.end());
        std::cout << part << std::endl;
    }

    // C++23
    auto split_view = std::ranges::split_view(s, std::string_view(","));
    for (auto &&subrange : split_view) {
        std::string part(subrange.begin(), subrange.end());
        std::cout << part << std::endl;
    }

    // C++98
    std::istringstream iss(s);
    std::string part;
    while (std::getline(iss, part, ',')) {
        std::cout << part << std::endl;
    }
    
    return 0;
}
*/

#include <atomic>
#include <thread>
#include <iostream>

int main() {
    std::atomic<int> counter{0};

    auto worker = [&]{
        for (int i = 0; i < 100'000; ++i) {
            int expected = counter.load(std::memory_order_relaxed);
            while (!counter.compare_exchange_weak(expected,
                                                  expected + 1,
                                                  std::memory_order_release,
                                                  std::memory_order_relaxed))
            {
                /* compare_exchange_weak 可能出现“伪失败”，
                   expected 会被更新为当前值，继续循环 */
            }
        }
    };

    std::thread t1(worker), t2(worker);
    t1.join(); t2.join();
    std::cout << "最终计数：" << counter << '\n';   // 200000
}