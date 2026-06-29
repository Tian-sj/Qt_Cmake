cmake_minimum_required(VERSION 3.25)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/GitVersion.cmake")
qtcpp_resolve_git_version()

project(
    QtCppTemplate
    VERSION ${QTCPP_VERSION_SEMVER}
    DESCRIPTION "A modular C++ and optional Qt desktop application template"
    LANGUAGES CXX
)

set(CMAKE_CXX_EXTENSIONS OFF)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not supported. Use: cmake --preset dev")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
include(ProjectOptions)

option(QTCPP_BUILD_GUI "Build the Qt desktop frontend" ON)
option(QTCPP_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(QTCPP_ENABLE_LICENSING "Enable the proprietary registration-code adapter" OFF)
option(QTCPP_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(QTCPP_ENABLE_CLANG_TIDY "Run clang-tidy while compiling" OFF)
option(QTCPP_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

if(QTCPP_ENABLE_LICENSING AND NOT QTCPP_BUILD_GUI)
    message(FATAL_ERROR "QTCPP_ENABLE_LICENSING requires QTCPP_BUILD_GUI because the third-party SDK uses Qt Core")
endif()

if(QTCPP_BUILD_GUI)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)
endif()

qtcpp_create_project_options()

set(QTCPP_ORGANIZATION "ExampleOrganization" CACHE STRING "Organization used for application metadata")
set(QTCPP_APPLICATION_NAME "QtCppTemplate" CACHE STRING "User-facing application name")
set(QTCPP_BUNDLE_IDENTIFIER "com.example.qtcpp-template" CACHE STRING "macOS bundle identifier")
if(APPLE)
    set(QTCPP_CODESIGN_IDENTITY "-" CACHE STRING "macOS code-signing identity; '-' means ad hoc")
endif()

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/project_config.hpp.in"
    "${PROJECT_BINARY_DIR}/generated/qtcpp/project_config.hpp"
    @ONLY
)

add_library(qtcpp_build_config INTERFACE)
add_library(QtCpp::build_config ALIAS qtcpp_build_config)
target_include_directories(
    qtcpp_build_config
    INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>"
)

if(QTCPP_ENABLE_LICENSING)
    add_subdirectory(third_party/registration_code)
endif()

add_subdirectory(src)

if(QTCPP_BUILD_GUI)
    add_subdirectory(apps/desktop)
endif()

if(QTCPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
