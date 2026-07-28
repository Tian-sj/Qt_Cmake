#pragma once

#include "cppproject/app_core/export.hpp"
#include "cppproject/app_core/ports.hpp"

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

namespace cppproject::app_core {

enum class ApplicationIssue {
    none,
    activation_required,
    clock_rollback,
    license_expired,
};

struct ApplicationStatus {
    ApplicationIssue issue{ApplicationIssue::none};
    LicenseStatus license_status{LicenseStatus::valid};
    std::optional<TimePoint> expires_at;
    bool expiration_warning{false};

    [[nodiscard]] bool can_continue() const noexcept {
        return issue == ApplicationIssue::none;
    }
};

struct ApplicationOptions {
    std::chrono::seconds clock_rollback_tolerance{5};
    std::chrono::hours expiration_warning_threshold{72};
};

class CPPPROJECT_APP_CORE_EXPORT ApplicationService final {
public:
    ApplicationService(SettingsStore& settings, Clock& clock,
                       LicenseGateway* license_gateway = nullptr, ApplicationOptions options = {});

    [[nodiscard]] ApplicationStatus start();
    [[nodiscard]] ApplicationStatus poll();
    [[nodiscard]] LicenseResult activate(std::string_view code);

    [[nodiscard]] bool licensing_enabled() const noexcept;
    [[nodiscard]] std::string machine_identifier() const;
    [[nodiscard]] std::string saved_license_code() const;
    void clear_license();

private:
    [[nodiscard]] bool clock_moved_backwards(TimePoint now) const;
    [[nodiscard]] bool should_warn(TimePoint now, const LicenseResult& result) const;
    void record_runtime(TimePoint now);

    SettingsStore& settings_;
    Clock& clock_;
    LicenseGateway* license_gateway_;
    ApplicationOptions options_;
    std::optional<TimePoint> last_runtime_;
    std::optional<LicenseResult> active_license_;
};

} // namespace cppproject::app_core
