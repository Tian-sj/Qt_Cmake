#include "cppproject/app_core/file_settings_store.hpp"
#include "test_support.hpp"

#include <chrono>
#include <filesystem>
#include <string>

namespace {

class TemporaryDirectory final {
public:
    TemporaryDirectory()
        : path_{std::filesystem::temp_directory_path() /
                ("cppproject-tests-" +
                 std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()))} {
        std::filesystem::create_directories(path_);
    }

    ~TemporaryDirectory() {
        std::error_code ignored;
        std::filesystem::remove_all(path_, ignored);
    }

    [[nodiscard]] const std::filesystem::path& path() const noexcept {
        return path_;
    }

private:
    std::filesystem::path path_;
};

void values_survive_reopening_and_preserve_utf8() {
    TemporaryDirectory directory;
    const auto file = directory.path() / "settings.conf";
    const std::string value = "中文配置=有效\n第二行%";

    {
        cppproject::app_core::FileSettingsStore settings{file};
        settings.set("用户.名称", value);
    }
    {
        cppproject::app_core::FileSettingsStore settings{file};
        cppproject::test::require_equal(settings.get("用户.名称").value_or(""), value,
                                        "UTF-8 value must round-trip");
        settings.remove("用户.名称");
        cppproject::test::require(!settings.get("用户.名称"), "removed setting must be absent");
    }
}

} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
    cppproject::test::Suite suite;
    suite.add("UTF-8 settings round-trip", values_survive_reopening_and_preserve_utf8);
    return suite.run();
}
