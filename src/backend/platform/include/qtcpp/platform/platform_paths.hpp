#pragma once

#include <filesystem>
#include <string_view>

namespace qtcpp::platform {

struct ApplicationIdentity {
    std::string_view organization;
    std::string_view application_name;
};

[[nodiscard]] std::filesystem::path
application_config_directory(const ApplicationIdentity& identity);

} // namespace qtcpp::platform
