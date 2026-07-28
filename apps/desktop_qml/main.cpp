#include "cppproject/app_core/application_service.hpp"
#include "cppproject/app_core/file_settings_store.hpp"
#include "cppproject/app_core/platform_paths.hpp"
#include "cppproject/app_core/system_clock.hpp"
#include "cppproject/project_config.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTimer>
#include <QVariant>
#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

QString from_utf8(const std::string_view value) {
    return QString::fromUtf8(value.data(), static_cast<qsizetype>(value.size()));
}

} // namespace

int main(int argc, char* argv[]) {
    QQuickWindow::setDefaultAlphaBuffer(true);
    QGuiApplication application{argc, argv};
    application.setApplicationName(from_utf8(cppproject::build::application_name) +
                                   QStringLiteral(" QML"));
    application.setOrganizationName(from_utf8(cppproject::build::organization));

    const auto settings_file = cppproject::app_core::application_config_directory(
                                   {.organization = cppproject::build::organization,
                                    .application_name = cppproject::build::application_name}) /
                               "qml-settings.conf";
    cppproject::app_core::FileSettingsStore settings{settings_file};
    cppproject::app_core::SystemClock clock;
    cppproject::app_core::ApplicationService app_core{settings, clock};
    const auto startup_status = app_core.start();
    if (!startup_status.can_continue()) {
        std::cerr << "QML application startup was blocked by app_core.\n";
        return EXIT_FAILURE;
    }

    const bool smoke_test = application.arguments().contains(QStringLiteral("--smoke-test"));
    QQmlApplicationEngine engine;
    engine.setInitialProperties(
        {{QStringLiteral("startupMessage"),
          QStringLiteral("app_core ready · %1").arg(from_utf8(cppproject::build::version))},
         {QStringLiteral("smokeTest"), smoke_test}});
    engine.loadFromModule(QStringLiteral("CppProject.Desktop"), QStringLiteral("Main"));
    if (engine.rootObjects().isEmpty()) {
        return EXIT_FAILURE;
    }

    if (smoke_test) {
        QTimer::singleShot(0, &application, &QCoreApplication::quit);
    }
    return application.exec();
}
