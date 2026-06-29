#pragma once

#include "qtcpp/application/ports.hpp"

#include <chrono>

namespace qtcpp::platform {

class SystemClock final : public application::Clock {
public:
    [[nodiscard]] application::TimePoint now() const override {
        return std::chrono::system_clock::now();
    }
};

} // namespace qtcpp::platform
