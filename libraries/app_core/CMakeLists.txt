add_library(cppproject_app_core)
add_library(CppProject::app_core ALIAS cppproject_app_core)

include(GenerateExportHeader)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/cppproject/app_core")
generate_export_header(
    cppproject_app_core
    BASE_NAME CPPPROJECT_APP_CORE
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/include/cppproject/app_core/export.hpp"
    STATIC_DEFINE CPPPROJECT_APP_CORE_STATIC_DEFINE
)

target_sources(
    cppproject_app_core
    PRIVATE
        src/application_service.cpp
        src/file_settings_store.cpp
        src/platform_paths.cpp
    PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS
            include
            "${CMAKE_CURRENT_BINARY_DIR}/include"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/include/cppproject/app_core/export.hpp"
            include/cppproject/app_core/application_service.hpp
            include/cppproject/app_core/file_settings_store.hpp
            include/cppproject/app_core/platform_paths.hpp
            include/cppproject/app_core/ports.hpp
            include/cppproject/app_core/system_clock.hpp
)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(cppproject_app_core PUBLIC CPPPROJECT_APP_CORE_STATIC_DEFINE)
endif()

target_link_libraries(
    cppproject_app_core
    PRIVATE
        "$<BUILD_INTERFACE:CppProject::project_options>"
        "$<BUILD_INTERFACE:CppProject::project_warnings>"
)

set_target_properties(
    cppproject_app_core
    PROPERTIES
        EXPORT_NAME app_core
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
        POSITION_INDEPENDENT_CODE ON
)

install(
    TARGETS cppproject_app_core
    EXPORT CppProjectTargets
    FILE_SET public_headers DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

if(CPPPROJECT_BUILD_TESTS)
    cppproject_add_test(
        cppproject_app_core_application_tests
        tests/application_service_tests.cpp
    )
    target_link_libraries(cppproject_app_core_application_tests PRIVATE CppProject::app_core)

    cppproject_add_test(
        cppproject_app_core_settings_tests
        tests/file_settings_store_tests.cpp
    )
    target_link_libraries(cppproject_app_core_settings_tests PRIVATE CppProject::app_core)
    set_tests_properties(
        cppproject_app_core_application_tests
        cppproject_app_core_settings_tests
        PROPERTIES LABELS app_core
    )
endif()
