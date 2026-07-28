#include "cppproject/app_core/platform_paths.hpp"

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace cppproject::app_core {
namespace {

std::filesystem::path environment_path(const char* variable) {
#if defined(_WIN32)
    const std::wstring wide_variable{variable, variable + std::char_traits<char>::length(variable)};
    wchar_t* value = nullptr;
    std::size_t value_size = 0;
    const auto result = _wdupenv_s(&value, &value_size, wide_variable.c_str());
    if (result == 0 && value != nullptr && value_size > 1U) {
        const std::filesystem::path path{value};
        std::free(value);
        return path;
    }
    std::free(value);
#else
    // 进程启动阶段只读取环境；运行期间禁止并发修改进程环境。
    // NOLINTNEXTLINE(concurrency-mt-unsafe)
    if (const char* value = std::getenv(variable); value != nullptr && value[0] != '\0') {
        return std::filesystem::path{value};
    }
#endif
    return {};
}

std::filesystem::path path_from_utf8(const std::string_view value) {
#if defined(_WIN32)
    const auto* begin = reinterpret_cast<const char8_t*>(value.data());
    return std::filesystem::path{std::u8string{begin, begin + value.size()}};
#else
    return std::filesystem::path{value};
#endif
}

std::filesystem::path home_directory() {
#if defined(_WIN32)
    auto home = environment_path("USERPROFILE");
#else
    auto home = environment_path("HOME");
#endif
    if (home.empty()) {
        throw std::runtime_error{"The user home directory is not available"};
    }
    return home;
}

} // namespace

std::filesystem::path application_config_directory(const ApplicationIdentity& identity) {
    const auto organization_path = path_from_utf8(identity.organization);
    const auto application_path = path_from_utf8(identity.application_name);

#if defined(_WIN32)
    auto root = environment_path("APPDATA");
    if (root.empty()) {
        root = home_directory() / "AppData" / "Roaming";
    }
    return root / organization_path / application_path;
#elif defined(__APPLE__)
    return home_directory() / "Library" / "Application Support" / organization_path /
           application_path;
#else
    auto root = environment_path("XDG_CONFIG_HOME");
    if (root.empty()) {
        root = home_directory() / ".config";
    }
    return root / organization_path / application_path;
#endif
}

} // namespace cppproject::app_core
