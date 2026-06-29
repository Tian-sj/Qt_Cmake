#include "qtcpp/application/application_service.hpp"
#include "qtcpp/gui/gui_application.hpp"
#include "qtcpp/platform/file_settings_store.hpp"
#include "qtcpp/platform/platform_paths.hpp"
#include "qtcpp/platform/system_clock.hpp"
#include "qtcpp/project_config.hpp"

#if defined(QTCPP_ENABLE_LICENSING)
#include "qtcpp/integrations/registration/registration_license_gateway.hpp"
#endif

#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char* argv[]) {
    try {
        const auto settings_file = qtcpp::platform::application_config_directory(
                                       {.organization = qtcpp::build::organization,
                                        .application_name = qtcpp::build::application_name}) /
                                   "settings.conf";

        qtcpp::platform::FileSettingsStore settings{settings_file};
        qtcpp::platform::SystemClock clock;

#if defined(QTCPP_ENABLE_LICENSING)
        const auto secret = std::string{qtcpp::build::organization} + "_" +
                            std::string{qtcpp::build::application_name};
        qtcpp::integrations::registration::RegistrationLicenseGateway license_gateway{secret};
        qtcpp::application::ApplicationService application{settings, clock, &license_gateway};
#else
        qtcpp::application::ApplicationService application{settings, clock};
#endif

        qtcpp::gui::GuiApplication gui{argc, argv, application};
        return gui.run();
    } catch (const std::exception& exception) {
        std::cerr << "Fatal startup error: " << exception.what() << '\n';
        return 1;
    }
}
