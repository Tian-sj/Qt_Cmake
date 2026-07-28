#include "cppproject/app_core/platform_paths.hpp"
#include "cppproject/plugin/plugin_api.h"
#include "cppproject/precision_countdown/signal.hpp"

int main() {
    const auto config_directory = cppproject::app_core::application_config_directory(
        {.organization = "Consumer", .application_name = "PackageTest"});

    cppproject::precision_countdown::Signal<int> signal;
    int observed = 0;
    auto connection = signal.connect([&observed](const int value) { observed = value; });
    signal.publish(7);

    return !config_directory.empty() && observed == 7 && connection.connected() &&
                   CPPPROJECT_PLUGIN_ABI_VERSION == 1u
               ? 0
               : 1;
}
