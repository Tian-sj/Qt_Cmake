#pragma once

#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cppproject::test {

class Suite final {
public:
    using Test = std::function<void()>;

    void add(std::string name, Test test) {
        tests_.emplace_back(std::move(name), std::move(test));
    }

    int run() const {
        int failures = 0;
        for (const auto& [name, test] : tests_) {
            try {
                test();
                std::cout << "[PASS] " << name << '\n';
            } catch (const std::exception& exception) {
                ++failures;
                std::cerr << "[FAIL] " << name << ": " << exception.what() << '\n';
            } catch (...) {
                ++failures;
                std::cerr << "[FAIL] " << name << ": unknown exception\n";
            }
        }
        return failures == 0 ? 0 : 1;
    }

private:
    std::vector<std::pair<std::string, Test>> tests_;
};

inline void require(const bool condition, const std::string_view message) {
    if (!condition) {
        throw std::runtime_error{std::string{message}};
    }
}

template <typename Actual, typename Expected>
void require_equal(const Actual& actual, const Expected& expected, const std::string_view message) {
    if (!(actual == expected)) {
        throw std::runtime_error{std::string{message}};
    }
}

} // namespace cppproject::test
