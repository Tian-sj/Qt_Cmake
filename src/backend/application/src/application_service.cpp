#include "qtcpp/application/application_service.hpp"

#include <charconv>
#include <chrono>
#include <cstdint>
#include <system_error>

namespace qtcpp::application {
namespace {

constexpr std::string_view license_code_key{"license.code"};
constexpr std::string_view last_runtime_key{"application.last_runtime"};

std::optional<TimePoint> deserialize_time(const std::optional<std::string>& value) {
    if (!value || value->empty()) {
        return std::nullopt;
    }

    std::int64_t seconds{};
    const auto* begin = value->data();
    const auto* end = begin + value->size();
    const auto [position, error] = std::from_chars(begin, end, seconds);
    if (error != std::errc{} || position != end) {
        return std::nullopt;
    }

    return TimePoint{std::chrono::seconds{seconds}};
}

std::string serialize_time(const TimePoint value) {
    const auto seconds =
        std::chrono::duration_cast<std::chrono::seconds>(value.time_since_epoch()).count();
    return std::to_string(seconds);
}

ApplicationStatus status_from_license(const LicenseResult& result, const bool expiration_warning) {
    ApplicationStatus status;
    status.license_status = result.status;
    status.expires_at = result.expires_at;
    status.expiration_warning = expiration_warning;

    if (result.status == LicenseStatus::expired) {
        status.issue = ApplicationIssue::license_expired;
    } else if (!result.accepted()) {
        status.issue = ApplicationIssue::activation_required;
    }

    return status;
}

} // namespace

ApplicationService::ApplicationService(SettingsStore& settings, Clock& clock,
                                       LicenseGateway* license_gateway, ApplicationOptions options)
    : settings_{settings}, clock_{clock}, license_gateway_{license_gateway}, options_{options},
      last_runtime_{deserialize_time(settings_.get(last_runtime_key))} {}

ApplicationStatus ApplicationService::start() {
    const auto current_time = clock_.now();
    if (clock_moved_backwards(current_time)) {
        return {.issue = ApplicationIssue::clock_rollback};
    }
    record_runtime(current_time);

    if (!licensing_enabled()) {
        return {};
    }

    const auto code = saved_license_code();
    if (code.empty()) {
        return {.issue = ApplicationIssue::activation_required,
                .license_status = LicenseStatus::invalid_format};
    }

    active_license_ = license_gateway_->validate(code);
    return status_from_license(*active_license_, should_warn(current_time, *active_license_));
}

ApplicationStatus ApplicationService::poll() {
    const auto current_time = clock_.now();
    if (clock_moved_backwards(current_time)) {
        return {.issue = ApplicationIssue::clock_rollback};
    }
    record_runtime(current_time);

    if (!active_license_) {
        return {};
    }

    if (active_license_->expires_at && current_time >= *active_license_->expires_at) {
        auto expired = *active_license_;
        expired.status = LicenseStatus::expired;
        return status_from_license(expired, false);
    }

    return status_from_license(*active_license_, should_warn(current_time, *active_license_));
}

LicenseResult ApplicationService::activate(const std::string_view code) {
    if (!licensing_enabled()) {
        return {.status = LicenseStatus::valid};
    }

    auto result = license_gateway_->validate(code);
    if (result.accepted()) {
        settings_.set(license_code_key, code);
        active_license_ = result;
    }
    return result;
}

bool ApplicationService::licensing_enabled() const noexcept {
    return license_gateway_ != nullptr;
}

std::string ApplicationService::machine_identifier() const {
    return licensing_enabled() ? license_gateway_->machine_identifier() : std::string{};
}

std::string ApplicationService::saved_license_code() const {
    return settings_.get(license_code_key).value_or(std::string{});
}

void ApplicationService::clear_license() {
    settings_.remove(license_code_key);
    active_license_.reset();
}

bool ApplicationService::clock_moved_backwards(const TimePoint now) const {
    return last_runtime_ && now + options_.clock_rollback_tolerance < *last_runtime_;
}

bool ApplicationService::should_warn(const TimePoint now, const LicenseResult& result) const {
    if (!result.accepted()) {
        return false;
    }
    if (result.status == LicenseStatus::about_to_expire) {
        return true;
    }
    return result.expires_at && *result.expires_at <= now + options_.expiration_warning_threshold;
}

void ApplicationService::record_runtime(const TimePoint now) {
    if (!last_runtime_ || now > *last_runtime_) {
        last_runtime_ = now;
        settings_.set(last_runtime_key, serialize_time(now));
    }
}

} // namespace qtcpp::application
