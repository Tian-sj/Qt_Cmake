#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

namespace qtcpp::application {

using TimePoint = std::chrono::system_clock::time_point;

enum class LicenseStatus {
    valid,
    about_to_expire,
    invalid_format,
    expired,
    invalid,
};

struct LicenseResult {
    LicenseStatus status{LicenseStatus::invalid};
    std::optional<TimePoint> expires_at;

    [[nodiscard]] bool accepted() const noexcept {
        return status == LicenseStatus::valid || status == LicenseStatus::about_to_expire;
    }
};

class LicenseGateway {
public:
    virtual ~LicenseGateway() = default;

    [[nodiscard]] virtual std::string machine_identifier() const = 0;
    [[nodiscard]] virtual LicenseResult validate(std::string_view code) = 0;
};

class SettingsStore {
public:
    virtual ~SettingsStore() = default;

    [[nodiscard]] virtual std::optional<std::string> get(std::string_view key) const = 0;
    virtual void set(std::string_view key, std::string_view value) = 0;
    virtual void remove(std::string_view key) = 0;
};

class Clock {
public:
    virtual ~Clock() = default;

    [[nodiscard]] virtual TimePoint now() const = 0;
};

} // namespace qtcpp::application
