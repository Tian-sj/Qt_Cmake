cmake_minimum_required(VERSION 3.25)

# 在 project() 之前解析 Git 版本，使 CMake 项目版本与生成的程序版本保持一致。
include("${CMAKE_CURRENT_LIST_DIR}/cmake/GitVersion.cmake")
qtcpp_resolve_git_version()

project(
    QtCppTemplate
    VERSION ${QTCPP_VERSION_SEMVER}
    DESCRIPTION "A modular C++ and optional Qt desktop application template"
    LANGUAGES CXX
)

# 本项目统一使用标准 C++20，不启用 gnu++ 等编译器私有语言扩展。
set(CMAKE_CXX_EXTENSIONS OFF)

# 禁止在源码目录直接构建，避免缓存和生成文件污染 Git 工作区。
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not supported. Use: cmake --preset dev")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
include(ProjectOptions)

# 可由 CMake Preset 或命令行覆盖的项目功能开关。
option(QTCPP_BUILD_GUI "Build the Qt desktop frontend" ON)
option(QTCPP_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(QTCPP_ENABLE_LICENSING "Enable the proprietary registration-code adapter" OFF)
option(QTCPP_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(QTCPP_ENABLE_CLANG_TIDY "Run clang-tidy while compiling" OFF)
option(QTCPP_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
set(QTCPP_EXTERNAL_THIRD_PARTY_ROOT "" CACHE PATH
    "Optional shared third-party repository containing third_party.cmake")

# 共享依赖位于项目仓库之外，只注册按需加载函数，不默认引入任何库或 include 路径。
if(NOT QTCPP_EXTERNAL_THIRD_PARTY_ROOT STREQUAL "")
    get_filename_component(
        QTCPP_EXTERNAL_THIRD_PARTY_ROOT
        "${QTCPP_EXTERNAL_THIRD_PARTY_ROOT}"
        REALPATH
    )
    set(qtcpp_external_third_party_index
        "${QTCPP_EXTERNAL_THIRD_PARTY_ROOT}/third_party.cmake")
    if(NOT EXISTS "${qtcpp_external_third_party_index}")
        message(FATAL_ERROR
            "QTCPP_EXTERNAL_THIRD_PARTY_ROOT must contain third_party.cmake: "
            "${QTCPP_EXTERNAL_THIRD_PARTY_ROOT}")
    endif()
    include("${qtcpp_external_third_party_index}")
    message(STATUS "Shared third-party repository: ${QTCPP_EXTERNAL_THIRD_PARTY_ROOT}")
endif()

# 根目录 bin 只保存正式程序和运行时文件；开发期产物仍隔离在当前 Preset 的 build 中。
# 测试程序会在 tests/CMakeLists.txt 中覆盖输出路径，避免混入正式运行目录。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/symbols")
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/symbols")
file(MAKE_DIRECTORY
    "${PROJECT_SOURCE_DIR}/bin"
    "${PROJECT_BINARY_DIR}/lib"
    "${PROJECT_BINARY_DIR}/symbols"
)

# 注册码 SDK 本身依赖 Qt Core，因此不能用于完全关闭 Qt 的构建。
if(QTCPP_ENABLE_LICENSING AND NOT QTCPP_BUILD_GUI)
    message(FATAL_ERROR "QTCPP_ENABLE_LICENSING requires QTCPP_BUILD_GUI because the third-party SDK uses Qt Core")
endif()

# 纯 C++ 模式不会查找 Qt，便于后端模块独立构建和测试。
if(QTCPP_BUILD_GUI)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)
endif()

qtcpp_create_project_options()

# 应用元数据进入生成头、Windows 版本资源和 macOS Bundle 信息。
set(QTCPP_ORGANIZATION "ExampleOrganization" CACHE STRING "Organization used for application metadata")
set(QTCPP_APPLICATION_NAME "QtCppTemplate" CACHE STRING "User-facing application name")
set(QTCPP_BUNDLE_IDENTIFIER "com.example.qtcpp-template" CACHE STRING "macOS bundle identifier")
if(APPLE)
    set(QTCPP_CODESIGN_IDENTITY "-" CACHE STRING "macOS code-signing identity; '-' means ad hoc")
endif()

# 生成只读的构建信息头，业务代码不需要直接读取 CMake 变量。
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/project_config.hpp.in"
    "${PROJECT_BINARY_DIR}/generated/qtcpp/project_config.hpp"
    @ONLY
)

# INTERFACE target 统一向需要版本信息的目标公开生成头目录。
add_library(qtcpp_build_config INTERFACE)
add_library(QtCpp::build_config ALIAS qtcpp_build_config)
target_include_directories(
    qtcpp_build_config
    INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>"
)

# 第三方 SDK 必须先定义 imported target，后端 integration 才能链接它。
if(QTCPP_ENABLE_LICENSING)
    add_subdirectory(vendor/registration_code)
endif()

# 源码、最终应用和测试按依赖顺序加入工程。
add_subdirectory(src)

if(QTCPP_BUILD_GUI)
    add_subdirectory(apps/desktop)
endif()

if(QTCPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
