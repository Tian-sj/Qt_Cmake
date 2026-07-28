cmake_minimum_required(VERSION 3.25)

# 在 project() 之前解析 Git 版本，使 CMake 项目版本与生成的程序版本保持一致。
include("${CMAKE_CURRENT_LIST_DIR}/cmake/GitVersion.cmake")
cppproject_resolve_git_version()

project(
    CppProjectTemplate
    VERSION ${CPPPROJECT_VERSION_SEMVER}
    DESCRIPTION "A cross-platform template for C++ and Qt libraries, runtime plugins, and Qt applications"
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
include(CMakePackageConfigHelpers)
include(ProjectOptions)

# 可由 CMake Preset 或 IDE 的 CMake 配置界面覆盖的项目功能开关。
option(BUILD_SHARED_LIBS "Build reusable libraries as shared libraries" OFF)
option(CPPPROJECT_BUILD_WIDGETS_APP "Build the Qt Widgets desktop application" ON)
option(CPPPROJECT_BUILD_QML_APP "Build the Qt Quick/QML desktop application" OFF)
set(cppproject_build_qt_libraries_default OFF)
if(CPPPROJECT_BUILD_WIDGETS_APP OR CPPPROJECT_BUILD_QML_APP)
    set(cppproject_build_qt_libraries_default ON)
endif()
option(
    CPPPROJECT_BUILD_QT_LIBRARIES
    "Build reusable libraries based on Qt"
    ${cppproject_build_qt_libraries_default}
)
set(cppproject_enable_qt_third_party_default OFF)
if(CPPPROJECT_BUILD_WIDGETS_APP OR CPPPROJECT_BUILD_QML_APP)
    set(cppproject_enable_qt_third_party_default ON)
endif()
option(
    CPPPROJECT_ENABLE_QT_THIRD_PARTY
    "Provide QCustomPlot, QXlsx, and QWindowKit targets"
    ${cppproject_enable_qt_third_party_default}
)
set(
    CPPPROJECT_QT_THIRD_PARTY_MODE
    "AUTO"
    CACHE STRING
    "Qt third-party mode: AUTO uses a matching cache or builds sources, SOURCE always builds sources, PREBUILT requires a matching cache"
)
set_property(
    CACHE CPPPROJECT_QT_THIRD_PARTY_MODE
    PROPERTY STRINGS AUTO SOURCE PREBUILT
)
string(TOUPPER
    "${CPPPROJECT_QT_THIRD_PARTY_MODE}"
    cppproject_qt_third_party_mode
)
if(NOT cppproject_qt_third_party_mode MATCHES "^(AUTO|SOURCE|PREBUILT)$")
    message(FATAL_ERROR
        "CPPPROJECT_QT_THIRD_PARTY_MODE must be AUTO, SOURCE, or PREBUILT")
endif()
set(
    CPPPROJECT_QT_THIRD_PARTY_MODE
    "${cppproject_qt_third_party_mode}"
    CACHE STRING
    "Qt third-party mode: AUTO uses a matching cache or builds sources, SOURCE always builds sources, PREBUILT requires a matching cache"
    FORCE
)
option(CPPPROJECT_BUILD_PLUGINS "Build the example runtime plugin" OFF)
option(CPPPROJECT_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(CPPPROJECT_ENABLE_LICENSING "Enable the proprietary registration-code adapter" OFF)
option(CPPPROJECT_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(CPPPROJECT_ENABLE_CLANG_TIDY "Run clang-tidy while compiling" OFF)
option(CPPPROJECT_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
set(CPPPROJECT_EXTERNAL_THIRD_PARTY_ROOT "" CACHE PATH
    "Optional shared third-party repository containing third_party.cmake")
mark_as_advanced(
    CPPPROJECT_WARNINGS_AS_ERRORS
    CPPPROJECT_ENABLE_CLANG_TIDY
    CPPPROJECT_ENABLE_SANITIZERS
    CPPPROJECT_EXTERNAL_THIRD_PARTY_ROOT
)

# 共享依赖位于项目仓库之外，只注册按需加载函数，不默认引入任何库或 include 路径。
if(NOT CPPPROJECT_EXTERNAL_THIRD_PARTY_ROOT STREQUAL "")
    get_filename_component(
        CPPPROJECT_EXTERNAL_THIRD_PARTY_ROOT
        "${CPPPROJECT_EXTERNAL_THIRD_PARTY_ROOT}"
        REALPATH
    )
    set(cppproject_external_third_party_index
        "${CPPPROJECT_EXTERNAL_THIRD_PARTY_ROOT}/third_party.cmake")
    if(NOT EXISTS "${cppproject_external_third_party_index}")
        message(FATAL_ERROR
            "CPPPROJECT_EXTERNAL_THIRD_PARTY_ROOT must contain third_party.cmake: "
            "${CPPPROJECT_EXTERNAL_THIRD_PARTY_ROOT}")
    endif()
    include("${cppproject_external_third_party_index}")
    message(STATUS "Shared third-party repository: ${CPPPROJECT_EXTERNAL_THIRD_PARTY_ROOT}")
endif()

# 每个 Preset 使用自己的运行目录，避免 Debug、Release 和不同 Qt Kit 相互覆盖。
# 测试程序会覆盖输出路径，避免混入正式运行目录。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/symbols")
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/symbols")
file(MAKE_DIRECTORY
    "${PROJECT_BINARY_DIR}/bin"
    "${PROJECT_BINARY_DIR}/lib"
    "${PROJECT_BINARY_DIR}/symbols"
)

# 注册码 SDK 本身依赖 Qt Core，因此不能用于完全关闭 Qt 的构建。
if(CPPPROJECT_ENABLE_LICENSING AND NOT CPPPROJECT_BUILD_WIDGETS_APP)
    message(FATAL_ERROR "CPPPROJECT_ENABLE_LICENSING requires CPPPROJECT_BUILD_WIDGETS_APP because the third-party SDK uses Qt Core")
endif()
if(CPPPROJECT_BUILD_WIDGETS_APP AND NOT CPPPROJECT_ENABLE_QT_THIRD_PARTY)
    message(FATAL_ERROR
        "CPPPROJECT_BUILD_WIDGETS_APP requires CPPPROJECT_ENABLE_QT_THIRD_PARTY "
        "because the desktop window uses QWindowKit")
endif()
if(CPPPROJECT_BUILD_QML_APP AND NOT CPPPROJECT_ENABLE_QT_THIRD_PARTY)
    message(FATAL_ERROR
        "CPPPROJECT_BUILD_QML_APP requires CPPPROJECT_ENABLE_QT_THIRD_PARTY "
        "because the QML window uses QWindowKit Quick")
endif()

# 纯 C++ 模式不会查找 Qt；Qt 库只需要 Core，桌面应用再按需增加 GUI 组件。
if(CPPPROJECT_BUILD_QT_LIBRARIES
   OR CPPPROJECT_BUILD_WIDGETS_APP
   OR CPPPROJECT_BUILD_QML_APP
   OR CPPPROJECT_ENABLE_QT_THIRD_PARTY)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Core)
endif()
if(CPPPROJECT_ENABLE_QT_THIRD_PARTY)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Gui Widgets PrintSupport)
    if(Qt6Gui_VERSION VERSION_GREATER_EQUAL "6.10.0")
        find_package(Qt6 REQUIRED COMPONENTS GuiPrivate WidgetsPrivate)
    endif()
endif()
if(CPPPROJECT_BUILD_WIDGETS_APP)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets LinguistTools)
endif()
if(CPPPROJECT_BUILD_QML_APP)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Qml Quick QuickControls2)
    if(Qt6Quick_VERSION VERSION_GREATER_EQUAL "6.10.0")
        find_package(Qt6 REQUIRED COMPONENTS QuickPrivate)
    endif()
endif()

cppproject_create_project_options()

if(CPPPROJECT_BUILD_TESTS)
    enable_testing()
    include(ProjectTests)
endif()

# 应用元数据进入生成头、Windows 版本资源和 macOS Bundle 信息。
set(CPPPROJECT_ORGANIZATION "ExampleOrganization" CACHE STRING "Organization used for application metadata")
set(CPPPROJECT_APPLICATION_NAME "CppProjectTemplate" CACHE STRING "User-facing application name")
set(CPPPROJECT_BUNDLE_IDENTIFIER "com.example.cppproject-template" CACHE STRING "macOS bundle identifier")
if(APPLE)
    set(CPPPROJECT_CODESIGN_IDENTITY "-" CACHE STRING "macOS code-signing identity; '-' means ad hoc")
    mark_as_advanced(CPPPROJECT_CODESIGN_IDENTITY)
endif()

# 生成只读的构建信息头，业务代码不需要直接读取 CMake 变量。
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/project_config.hpp.in"
    "${PROJECT_BINARY_DIR}/generated/cppproject/project_config.hpp"
    @ONLY
)

# INTERFACE target 统一向需要版本信息的目标公开生成头目录。
add_library(cppproject_build_config INTERFACE)
add_library(CppProject::build_config ALIAS cppproject_build_config)
target_include_directories(
    cppproject_build_config
    INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>"
)

# 第三方 SDK 必须先定义 imported target，桌面应用的 adapter 才能链接它。
if(CPPPROJECT_ENABLE_LICENSING)
    add_subdirectory(vendor/registration_code)
endif()

# 可发布库和插件协议不依赖 Qt，可被其他 CMake 项目单独消费。
add_subdirectory(libraries/app_core)
add_subdirectory(libraries/precision_countdown)
add_subdirectory(plugins/api)

if(CPPPROJECT_BUILD_QT_LIBRARIES)
    add_subdirectory(libraries/qt_status)
endif()

if(CPPPROJECT_ENABLE_QT_THIRD_PARTY)
    add_subdirectory(libraries/qt_third_party)
endif()

if(CPPPROJECT_BUILD_PLUGINS)
    add_subdirectory(plugins/example)
endif()

if(CPPPROJECT_BUILD_WIDGETS_APP)
    add_subdirectory(apps/desktop_widgets)
endif()
if(CPPPROJECT_BUILD_QML_APP)
    add_subdirectory(apps/desktop_qml)
endif()

# 安装后的库可通过 find_package(CppProjectTemplate CONFIG REQUIRED) 使用。
set(cppproject_package_directory "${CMAKE_INSTALL_LIBDIR}/cmake/CppProjectTemplate")
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/CppProjectTemplateConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/CppProjectTemplateConfig.cmake"
    INSTALL_DESTINATION "${cppproject_package_directory}"
)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/CppProjectTemplateConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
)
install(
    EXPORT CppProjectTargets
    FILE CppProjectTemplateTargets.cmake
    NAMESPACE CppProject::
    DESTINATION "${cppproject_package_directory}"
)
install(
    FILES
        "${PROJECT_BINARY_DIR}/CppProjectTemplateConfig.cmake"
        "${PROJECT_BINARY_DIR}/CppProjectTemplateConfigVersion.cmake"
    DESTINATION "${cppproject_package_directory}"
)
