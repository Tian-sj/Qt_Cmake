#pragma once

#include "cppproject/app_core/ports.hpp"

#include <chrono>

namespace cppproject::app_core {

class SystemClock final : public Clock {
public:
    [[nodiscard]] TimePoint now() const override {
        return std::chrono::system_clock::now();
    }
};

} // namespace cppproject::app_core
