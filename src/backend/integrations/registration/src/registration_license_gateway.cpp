#include "qtcpp/integrations/registration/registration_license_gateway.hpp"

#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <chrono>
#include <registration_code/registration_code.hpp>
#include <utility>

namespace qtcpp::integrations::registration {
namespace {

// 第三方枚举只能在 integration 内出现，application 对外只暴露稳定的自有类型。
application::LicenseStatus to_application_status(const RegistrationCode::ErrorType status) {
    using Source = RegistrationCode::ErrorType;
    switch (status) {
    case Source::REGISTRATION_CODE_VALID:
        return application::LicenseStatus::valid;
    case Source::REGISTRATION_CODE_ABOUT_TO_EXPIRE:
        return application::LicenseStatus::about_to_expire;
    case Source::REGISTRATION_CODE_INVALID_FORMAT:
        return application::LicenseStatus::invalid_format;
    case Source::REGISTRATION_CODE_EXPIRED:
        return application::LicenseStatus::expired;
    case Source::REGISTRATION_CODE_INVALID:
        return application::LicenseStatus::invalid;
    }
    return application::LicenseStatus::invalid;
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

application::LicenseResult RegistrationLicenseGateway::validate(const std::string_view code) {
    const auto qt_code = QString::fromUtf8(code.data(), static_cast<qsizetype>(code.size()));
    const auto status = to_application_status(registration_code_->validateCode(qt_code));

    application::LicenseResult result{.status = status};
    if (status == application::LicenseStatus::valid ||
        status == application::LicenseStatus::about_to_expire) {
        const auto expiration = registration_code_->getEndTime();
        if (expiration.isValid()) {
            result.expires_at =
                application::TimePoint{std::chrono::seconds{expiration.toSecsSinceEpoch()}};
        }
    }
    return result;
}

} // namespace qtcpp::integrations::registration
