#include "cppproject/app_core/application_service.hpp"
#include "cppproject/app_core/file_settings_store.hpp"
#include "cppproject/app_core/platform_paths.hpp"
#include "cppproject/app_core/system_clock.hpp"
#include "cppproject/widgets/widgets_application.hpp"
#include "cppproject/project_config.hpp"

#if defined(CPPPROJECT_ENABLE_LICENSING)
#include "integrations/registration_license_gateway.hpp"
#endif

#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char* argv[]) {
    try {
        // main 是唯一组合根：在这里选择具体实现并通过构造函数注入应用层。
        const auto settings_file = cppproject::app_core::application_config_directory(
                                       {.organization = cppproject::build::organization,
                                        .application_name = cppproject::build::application_name}) /
                                   "settings.conf";

        cppproject::app_core::FileSettingsStore settings{settings_file};
        cppproject::app_core::SystemClock clock;

#if defined(CPPPROJECT_ENABLE_LICENSING)
        const auto secret = std::string{cppproject::build::organization} + "_" +
                            std::string{cppproject::build::application_name};
        cppproject::widgets::RegistrationLicenseGateway license_gateway{secret};
        cppproject::app_core::ApplicationService application{settings, clock, &license_gateway};
#else
        cppproject::app_core::ApplicationService application{settings, clock};
#endif

        cppproject::widgets::WidgetsApplication gui{argc, argv, application};
        return gui.run();
    } catch (const std::exception& exception) {
        std::cerr << "Fatal startup error: " << exception.what() << '\n';
        return 1;
    }
}
