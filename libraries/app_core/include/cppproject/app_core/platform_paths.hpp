#pragma once

#include "cppproject/app_core/export.hpp"

#include <filesystem>
#include <string_view>

namespace cppproject::app_core {

struct ApplicationIdentity {
    std::string_view organization;
    std::string_view application_name;
};

[[nodiscard]] CPPPROJECT_APP_CORE_EXPORT std::filesystem::path
application_config_directory(const ApplicationIdentity& identity);

} // namespace cppproject::app_core
