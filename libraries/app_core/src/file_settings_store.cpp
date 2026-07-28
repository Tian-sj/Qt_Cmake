#include "cppproject/app_core/file_settings_store.hpp"

#include <fstream>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <utility>

namespace cppproject::app_core {
namespace {

char hex_digit(const unsigned int value) {
    constexpr std::string_view digits{"0123456789ABCDEF"};
    return digits[value & 0xFU];
}

int hex_value(const char value) {
    if (value >= '0' && value <= '9') {
        return value - '0';
    }
    if (value >= 'A' && value <= 'F') {
        return value - 'A' + 10;
    }
    if (value >= 'a' && value <= 'f') {
        return value - 'a' + 10;
    }
    return -1;
}

bool is_unreserved(const unsigned char value) {
    return (value >= 'a' && value <= 'z') || (value >= 'A' && value <= 'Z') ||
           (value >= '0' && value <= '9') || value == '-' || value == '_' || value == '.';
}

// 对 UTF-8 原始字节做百分号编码，确保 '='、换行和 '%' 不破坏逐行文件格式。
std::string encode(const std::string_view value) {
    std::string encoded;
    encoded.reserve(value.size());
    for (const char character : value) {
        const auto byte = static_cast<unsigned char>(character);
        if (is_unreserved(byte)) {
            encoded.push_back(character);
        } else {
            encoded.push_back('%');
            encoded.push_back(hex_digit(byte >> 4U));
            encoded.push_back(hex_digit(byte));
        }
    }
    return encoded;
}

std::string decode(const std::string_view value, const std::size_t line_number) {
    std::string decoded;
    decoded.reserve(value.size());
    for (std::size_t index = 0; index < value.size(); ++index) {
        if (value[index] != '%') {
            decoded.push_back(value[index]);
            continue;
        }
        if (index + 2 >= value.size()) {
            throw std::runtime_error{"Invalid percent escape in settings file at line " +
                                     std::to_string(line_number)};
        }
        const auto high = hex_value(value[index + 1]);
        const auto low = hex_value(value[index + 2]);
        if (high < 0 || low < 0) {
            throw std::runtime_error{"Invalid percent escape in settings file at line " +
                                     std::to_string(line_number)};
        }
        decoded.push_back(static_cast<char>((high << 4) | low));
        index += 2;
    }
    return decoded;
}

} // namespace

FileSettingsStore::FileSettingsStore(std::filesystem::path file_path)
    : file_path_{std::move(file_path)} {
    load();
}

std::optional<std::string> FileSettingsStore::get(const std::string_view key) const {
    const std::scoped_lock lock{mutex_};
    const auto position = values_.find(key);
    if (position == values_.end()) {
        return std::nullopt;
    }
    return position->second;
}

void FileSettingsStore::set(const std::string_view key, const std::string_view value) {
    const std::scoped_lock lock{mutex_};
    values_.insert_or_assign(std::string{key}, std::string{value});
    persist_locked();
}

void FileSettingsStore::remove(const std::string_view key) {
    const std::scoped_lock lock{mutex_};
    const auto position = values_.find(key);
    if (position != values_.end()) {
        values_.erase(position);
        persist_locked();
    }
}

void FileSettingsStore::load() {
    const std::scoped_lock lock{mutex_};
    if (!std::filesystem::exists(file_path_)) {
        return;
    }

    std::ifstream input{file_path_, std::ios::binary};
    if (!input) {
        throw std::runtime_error{"Unable to open settings file: " + file_path_.string()};
    }

    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        if (line.empty()) {
            continue;
        }

        const auto separator = line.find('=');
        if (separator == std::string::npos) {
            throw std::runtime_error{"Invalid settings file at line " +
                                     std::to_string(line_number)};
        }
        auto key = decode(std::string_view{line}.substr(0, separator), line_number);
        auto value = decode(std::string_view{line}.substr(separator + 1), line_number);
        values_.insert_or_assign(std::move(key), std::move(value));
    }
}

void FileSettingsStore::persist_locked() const {
    const auto parent = file_path_.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }

    auto temporary_file = file_path_;
    temporary_file += ".tmp";

    // 先完整写入临时文件，再替换正式文件，避免异常中断留下半份配置。
    {
        std::ofstream output{temporary_file, std::ios::binary | std::ios::trunc};
        if (!output) {
            throw std::runtime_error{"Unable to write settings file: " + temporary_file.string()};
        }
        for (const auto& [key, value] : values_) {
            output << encode(key) << '=' << encode(value) << '\n';
        }
        output.flush();
        if (!output) {
            throw std::runtime_error{"Failed while writing settings file: " +
                                     temporary_file.string()};
        }
    }

    std::error_code error;
    std::filesystem::rename(temporary_file, file_path_, error);
    if (error) {
        // Windows 不允许 rename 覆盖已有文件，因此删除旧文件后重试。
        std::filesystem::remove(file_path_, error);
        error.clear();
        std::filesystem::rename(temporary_file, file_path_, error);
    }
    if (error) {
        std::filesystem::remove(temporary_file);
        throw std::runtime_error{"Unable to replace settings file: " + error.message()};
    }
}

} // namespace cppproject::app_core
