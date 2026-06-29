#pragma once

#include "qtcpp/application/ports.hpp"

#include <filesystem>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>

namespace qtcpp::platform {

class FileSettingsStore final : public application::SettingsStore {
public:
    explicit FileSettingsStore(std::filesystem::path file_path);

    [[nodiscard]] std::optional<std::string> get(std::string_view key) const override;
    void set(std::string_view key, std::string_view value) override;
    void remove(std::string_view key) override;

private:
    void load();
    void persist_locked() const;

    std::filesystem::path file_path_;
    mutable std::mutex mutex_;
    std::map<std::string, std::string, std::less<>> values_;
};

} // namespace qtcpp::platform
