#include "cppproject/plugin/plugin_api.h"

namespace {

const cppproject_host_api* host_api = nullptr;

int CPPPROJECT_PLUGIN_CALL initialize(const cppproject_host_api* host) {
    if (host == nullptr || host->abi_version != CPPPROJECT_PLUGIN_ABI_VERSION) {
        return 0;
    }

    host_api = host;
    if (host_api->log_message != nullptr) {
        host_api->log_message("Example plugin initialized");
    }
    return 1;
}

void CPPPROJECT_PLUGIN_CALL shutdown() {
    host_api = nullptr;
}

const cppproject_plugin_api plugin_api{
    CPPPROJECT_PLUGIN_ABI_VERSION,
    "cppproject.example",
    &initialize,
    &shutdown,
};

} // namespace

extern "C" CPPPROJECT_PLUGIN_EXPORT const cppproject_plugin_api* CPPPROJECT_PLUGIN_CALL
cppproject_plugin_get_api(const uint32_t host_abi_version) {
    return host_abi_version == CPPPROJECT_PLUGIN_ABI_VERSION ? &plugin_api : nullptr;
}
