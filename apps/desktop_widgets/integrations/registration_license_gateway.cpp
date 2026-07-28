#include "integrations/registration_license_gateway.hpp"

#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <chrono>
#include <registration_code/registration_code.hpp>
#include <utility>

namespace cppproject::widgets {
namespace {

// 第三方枚举只能在 integration 内出现，application 对外只暴露稳定的自有类型。
app_core::LicenseStatus to_application_status(const RegistrationCode::ErrorType status) {
    using Source = RegistrationCode::ErrorType;
    switch (status) {
    case Source::REGISTRATION_CODE_VALID:
        return app_core::LicenseStatus::valid;
    case Source::REGISTRATION_CODE_ABOUT_TO_EXPIRE:
        return app_core::LicenseStatus::about_to_expire;
    case Source::REGISTRATION_CODE_INVALID_FORMAT:
        return app_core::LicenseStatus::invalid_format;
    case Source::REGISTRATION_CODE_EXPIRED:
        return app_core::LicenseStatus::expired;
    case Source::REGISTRATION_CODE_INVALID:
        return app_core::LicenseStatus::invalid;
    }
    return app_core::LicenseStatus::invalid;
}

} // namespace

RegistrationLicenseGateway::RegistrationLicenseGateway(const std::string_view secret)
    : registration_code_{std::make_unique<RegistrationCode>(
          QByteArray{secret.data(), static_cast<qsizetype>(secret.size())})} {}

RegistrationLicenseGateway::~RegistrationLicenseGateway() = default;
RegistrationLicenseGateway::RegistrationLicenseGateway(RegistrationLicenseGateway&&) noexcept =
    default;
RegistrationLicenseGateway&
RegistrationLicenseGateway::operator=(RegistrationLicenseGateway&&) noexcept = default;

std::string RegistrationLicenseGateway::machine_identifier() const {
    return registration_code_->getUniqueSystemIdentifier().toUtf8().toStdString();
}

app_core::LicenseResult RegistrationLicenseGateway::validate(const std::string_view code) {
    const auto qt_code = QString::fromUtf8(code.data(), static_cast<qsizetype>(code.size()));
    const auto status = to_application_status(registration_code_->validateCode(qt_code));

    app_core::LicenseResult result{.status = status};
    if (status == app_core::LicenseStatus::valid ||
        status == app_core::LicenseStatus::about_to_expire) {
        const auto expiration = registration_code_->getEndTime();
        if (expiration.isValid()) {
            result.expires_at =
                app_core::TimePoint{std::chrono::seconds{expiration.toSecsSinceEpoch()}};
        }
    }
    return result;
}

} // namespace cppproject::widgets
