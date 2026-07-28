#pragma once

#include "cppproject/app_core/ports.hpp"

#include <memory>
#include <string>
#include <string_view>

class RegistrationCode;

namespace cppproject::desktop {

class RegistrationLicenseGateway final : public app_core::LicenseGateway {
public:
    explicit RegistrationLicenseGateway(std::string_view secret);
    ~RegistrationLicenseGateway() override;

    RegistrationLicenseGateway(const RegistrationLicenseGateway&) = delete;
    RegistrationLicenseGateway& operator=(const RegistrationLicenseGateway&) = delete;
    RegistrationLicenseGateway(RegistrationLicenseGateway&&) noexcept;
    RegistrationLicenseGateway& operator=(RegistrationLicenseGateway&&) noexcept;

    [[nodiscard]] std::string machine_identifier() const override;
    [[nodiscard]] app_core::LicenseResult validate(std::string_view code) override;

private:
    std::unique_ptr<RegistrationCode> registration_code_;
};

} // namespace cppproject::desktop
