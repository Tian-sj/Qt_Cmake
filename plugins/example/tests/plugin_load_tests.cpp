#include "cppproject/plugin/plugin_api.h"

#include <cstring>
#include <filesystem>
#include <iostream>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace {

bool logged = false;

void CPPPROJECT_PLUGIN_CALL log_message(const char* message) {
    logged = message != nullptr && message[0] != '\0';
}

class PluginLibrary final {
public:
    explicit PluginLibrary(const std::filesystem::path& path) {
#if defined(_WIN32)
        handle_ = LoadLibraryW(path.c_str());
#else
        handle_ = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
    }

    ~PluginLibrary() {
#if defined(_WIN32)
        if (handle_ != nullptr) {
            FreeLibrary(handle_);
        }
#else
        if (handle_ != nullptr) {
            dlclose(handle_);
        }
#endif
    }

    PluginLibrary(const PluginLibrary&) = delete;
    PluginLibrary& operator=(const PluginLibrary&) = delete;

    [[nodiscard]] bool loaded() const noexcept {
        return handle_ != nullptr;
    }

    [[nodiscard]] cppproject_plugin_get_api_fn get_api() const noexcept {
#if defined(_WIN32)
        const auto symbol = GetProcAddress(handle_, "cppproject_plugin_get_api");
#else
        const auto symbol = dlsym(handle_, "cppproject_plugin_get_api");
#endif
        cppproject_plugin_get_api_fn function = nullptr;
        static_assert(sizeof(function) == sizeof(symbol));
        std::memcpy(&function, &symbol, sizeof(function));
        return function;
    }

private:
#if defined(_WIN32)
    HMODULE handle_{nullptr};
#else
    void* handle_{nullptr};
#endif
};

} // namespace

int main(const int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Expected plugin path\n";
        return 1;
    }

    PluginLibrary library{std::filesystem::path{argv[1]}};
    if (!library.loaded()) {
        std::cerr << "Unable to load plugin\n";
        return 2;
    }

    const auto get_api = library.get_api();
    if (get_api == nullptr || get_api(CPPPROJECT_PLUGIN_ABI_VERSION + 1u) != nullptr) {
        std::cerr << "Plugin ABI negotiation failed\n";
        return 3;
    }

    const auto* plugin = get_api(CPPPROJECT_PLUGIN_ABI_VERSION);
    if (plugin == nullptr || plugin->abi_version != CPPPROJECT_PLUGIN_ABI_VERSION ||
        plugin->name == nullptr || plugin->initialize == nullptr || plugin->shutdown == nullptr) {
        std::cerr << "Plugin API is incomplete\n";
        return 4;
    }

    const cppproject_host_api host{CPPPROJECT_PLUGIN_ABI_VERSION, &log_message};
    if (plugin->initialize(&host) == 0 || !logged) {
        std::cerr << "Plugin initialization failed\n";
        return 5;
    }

    plugin->shutdown();
    return 0;
}
