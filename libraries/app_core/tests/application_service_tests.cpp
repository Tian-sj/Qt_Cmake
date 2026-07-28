#include "cppproject/app_core/application_service.hpp"
#include "test_support.hpp"

#include <chrono>
#include <map>
#include <optional>
#include <string>
#include <string_view>

namespace {

using namespace std::chrono_literals;
using cppproject::app_core::ApplicationIssue;
using cppproject::app_core::ApplicationService;
using cppproject::app_core::LicenseResult;
using cppproject::app_core::LicenseStatus;
using cppproject::app_core::TimePoint;

class MemorySettings final : public cppproject::app_core::SettingsStore {
public:
    std::optional<std::string> get(const std::string_view key) const override {
        const auto position = values.find(key);
        return position == values.end() ? std::nullopt : std::optional{position->second};
    }

    void set(const std::string_view key, const std::string_view value) override {
        values.insert_or_assign(std::string{key}, std::string{value});
    }

    void remove(const std::string_view key) override {
        values.erase(std::string{key});
    }

    std::map<std::string, std::string, std::less<>> values;
};

class FakeClock final : public cppproject::app_core::Clock {
public:
    TimePoint now() const override {
        return current;
    }
    TimePoint current{TimePoint{1000s}};
};

class FakeLicenseGateway final : public cppproject::app_core::LicenseGateway {
public:
    std::string machine_identifier() const override {
        return "machine-123";
    }
    LicenseResult validate(std::string_view) override {
        return next_result;
    }

    LicenseResult next_result{.status = LicenseStatus::valid, .expires_at = TimePoint{10000s}};
};

void no_license_mode_starts_without_qt_or_activation() {
    MemorySettings settings;
    FakeClock clock;
    ApplicationService service{settings, clock};

    const auto status = service.start();
    cppproject::test::require(status.can_continue(), "pure C++ mode should start");
    cppproject::test::require(!service.licensing_enabled(), "license should be optional");
    cppproject::test::require(settings.get("application.last_runtime").has_value(),
                              "runtime must persist");
}

void missing_license_requires_activation_and_success_is_persisted() {
    MemorySettings settings;
    FakeClock clock;
    FakeLicenseGateway license;
    ApplicationService service{settings, clock, &license};

    const auto startup = service.start();
    cppproject::test::require_equal(startup.issue, ApplicationIssue::activation_required,
                                    "missing code must activate");

    const auto result = service.activate("valid-code");
    cppproject::test::require(result.accepted(), "valid code should be accepted");
    cppproject::test::require_equal(settings.get("license.code").value_or(""),
                                    std::string{"valid-code"}, "code must persist");
}

void a_clock_rollback_is_rejected() {
    MemorySettings settings;
    settings.set("application.last_runtime", "2000");
    FakeClock clock;
    ApplicationService service{settings, clock};

    const auto status = service.start();
    cppproject::test::require_equal(status.issue, ApplicationIssue::clock_rollback,
                                    "rollback must be detected");
}

void expiration_warning_and_expiration_are_reported() {
    MemorySettings settings;
    settings.set("license.code", "saved-code");
    FakeClock clock;
    FakeLicenseGateway license;
    license.next_result = {
        .status = LicenseStatus::valid,
        .expires_at = clock.current + 2h,
    };
    ApplicationService service{settings, clock, &license};

    const auto startup = service.start();
    cppproject::test::require(startup.expiration_warning, "expiry inside 72 hours should warn");

    clock.current += 3h;
    const auto status = service.poll();
    cppproject::test::require_equal(status.issue, ApplicationIssue::license_expired,
                                    "expired license must stop the app");
}

} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
    cppproject::test::Suite suite;
    suite.add("no-license mode", no_license_mode_starts_without_qt_or_activation);
    suite.add("activation persists", missing_license_requires_activation_and_success_is_persisted);
    suite.add("clock rollback", a_clock_rollback_is_rejected);
    suite.add("license expiration", expiration_warning_and_expiration_are_reported);
    return suite.run();
}
