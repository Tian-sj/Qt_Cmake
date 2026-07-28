# 桌面 UI 保持为独立 target，便于窗口代码单独编译，也避免入口文件承载界面实现。
qt_add_library(cppproject_desktop_ui STATIC
    include/cppproject/desktop/desktop_application.hpp
    application/desktop_application.cpp
    application/ui_settings.cpp
    application/ui_settings.hpp
    views/main_window.cpp
    views/main_window.hpp
    views/main_window.ui
    views/registration_code_dialog.cpp
    views/registration_code_dialog.hpp
    views/registration_code_dialog.ui
    views/registration_expiration_reminder.cpp
    views/registration_expiration_reminder.hpp
    views/registration_expiration_reminder.ui
    widgets/drag_drop_plain_text_edit_rc.cpp
    widgets/drag_drop_plain_text_edit_rc.hpp
    resources/resource.qrc
)

add_library(CppProject::desktop_ui ALIAS cppproject_desktop_ui)

set_target_properties(
    cppproject_desktop_ui
    PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
        AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/views"
)

target_include_directories(
    cppproject_desktop_ui
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_link_libraries(
    cppproject_desktop_ui
    PUBLIC
        CppProject::app_core
    PRIVATE
        CppProject::qwindowkit
        Qt6::Widgets
        CppProject::build_config
        CppProject::project_options
        CppProject::project_warnings
)

qt_add_translations(
    cppproject_desktop_ui
    TS_FILES
        translations/desktop_zh_CN.ts
    RESOURCE_PREFIX "/i18n"
    LUPDATE_OPTIONS -no-obsolete
)

# 平台图标属于最终应用资源，不进入应用核心或桌面 UI 静态库。
set(app_icon)

if(WIN32)
    set(app_icon "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.rc.in" "${app_icon}" @ONLY)
elseif(APPLE)
    set(app_icon "${PROJECT_SOURCE_DIR}/apps/desktop/assets/icons/logo.icns")
    set_source_files_properties("${app_icon}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

# desktop 是组合根：它只负责链接各模块并生成最终可执行程序。
if(CPPPROJECT_ENABLE_LICENSING)
    add_library(cppproject_registration_integration STATIC
        integrations/registration_license_gateway.cpp
        integrations/registration_license_gateway.hpp
    )
    target_include_directories(
        cppproject_registration_integration
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    target_link_libraries(
        cppproject_registration_integration
        PUBLIC CppProject::app_core
        PRIVATE
            CppProject::project_options
            CppProject::project_warnings
            CppProject::registration_sdk
    )
endif()

qt_add_executable(cppproject_desktop
    main.cpp
    ${app_icon}
)

add_executable(CppProject::desktop ALIAS cppproject_desktop)

# 依赖注入发生在 main.cpp，因此具体实现 target 只由最终应用链接。
target_link_libraries(
    cppproject_desktop
    PRIVATE
        CppProject::app_core
        CppProject::build_config
        CppProject::desktop_ui
        CppProject::project_options
        CppProject::project_warnings
)

# 授权功能关闭时，编译器不会看到第三方 SDK 的任何类型或代码路径。
if(CPPPROJECT_ENABLE_LICENSING)
    target_link_libraries(cppproject_desktop PRIVATE cppproject_registration_integration)
    target_compile_definitions(cppproject_desktop PRIVATE CPPPROJECT_ENABLE_LICENSING=1)
endif()

# 同一组版本变量同时驱动 Windows 资源和 macOS Bundle 元数据。
set_target_properties(
    cppproject_desktop
    PROPERTIES
        OUTPUT_NAME "${CPPPROJECT_APPLICATION_NAME}"
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "${CPPPROJECT_APPLICATION_NAME}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "${CPPPROJECT_BUNDLE_IDENTIFIER}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${CPPPROJECT_VERSION_SEMVER}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${CPPPROJECT_VERSION_BUILD}"
)

# 字体是运行时资源：macOS 放入 Bundle，Windows/Linux 放在程序旁的 fonts 目录。
if(APPLE)
    set(font_output_directory "$<TARGET_BUNDLE_CONTENT_DIR:cppproject_desktop>/Resources/fonts")
else()
    set(font_output_directory "$<TARGET_FILE_DIR:cppproject_desktop>/fonts")
endif()

add_custom_command(
    TARGET cppproject_desktop
    POST_BUILD
    COMMAND
        "${CMAKE_COMMAND}" -E copy_directory
        "${PROJECT_SOURCE_DIR}/apps/desktop/assets/fonts"
        "${font_output_directory}"
    VERBATIM
)

# Windows 构建后只部署该程序实际依赖的 Qt DLL 和插件。
# 项目翻译已经编译进 qrc，无需复制整套 Qt 翻译文件。
if(WIN32)
    add_custom_command(
        TARGET cppproject_desktop
        POST_BUILD
        COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_RUNTIME_DLLS:cppproject_desktop>"
            "$<TARGET_FILE_DIR:cppproject_desktop>"
        COMMAND_EXPAND_LISTS
        COMMENT "Copying dependent runtime libraries"
        VERBATIM
    )

    if(NOT TARGET Qt6::windeployqt)
        message(FATAL_ERROR "Qt Kit 中未找到 windeployqt，无法准备 bin 运行目录。")
    endif()
    add_custom_command(
        TARGET cppproject_desktop
        POST_BUILD
        COMMAND
            "$<TARGET_FILE:Qt6::windeployqt>"
            --dir "$<TARGET_FILE_DIR:cppproject_desktop>"
            --no-translations
            "$<TARGET_FILE:cppproject_desktop>"
        COMMENT "Deploying required Qt runtime files to bin"
        VERBATIM
    )
endif()

# install() 生成可发布目录；当前 Preset 的 bin 用于本机直接运行和调试。
install(
    TARGETS cppproject_desktop
    BUNDLE DESTINATION .
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

if(NOT APPLE)
    install(
        DIRECTORY "${PROJECT_SOURCE_DIR}/apps/desktop/assets/fonts/"
        DESTINATION "${CMAKE_INSTALL_BINDIR}/fonts"
        FILES_MATCHING PATTERN "*.ttf" PATTERN "LICENSE.txt"
    )
endif()

# 专有 SDK 的运行库由 third_party 模块按平台复制到正确位置。
if(CPPPROJECT_ENABLE_LICENSING)
    cppproject_install_registration_runtime(cppproject_desktop)
endif()

# Qt 官方部署脚本在安装阶段补齐框架、插件和平台运行库。
set(cppproject_deploy_tool_options)
if(APPLE AND NOT CPPPROJECT_CODESIGN_IDENTITY STREQUAL "")
    list(APPEND cppproject_deploy_tool_options "-codesign=${CPPPROJECT_CODESIGN_IDENTITY}")
endif()

qt_generate_deploy_app_script(
    TARGET cppproject_desktop
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_TOOL_OPTIONS ${cppproject_deploy_tool_options}
)
install(SCRIPT "${deploy_script}")
