# 平台图标属于最终应用资源，不进入后端或 Qt GUI 静态库。
set(app_icon)

if(WIN32)
    set(app_icon "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.rc.in" "${app_icon}" @ONLY)
elseif(APPLE)
    set(app_icon "${PROJECT_SOURCE_DIR}/src/frontend/qt/assets/icons/logo.icns")
    set_source_files_properties("${app_icon}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

# desktop 是组合根：它只负责链接各模块并生成最终可执行程序。
qt_add_executable(qtcpp_desktop
    main.cpp
    ${app_icon}
)

add_executable(QtCpp::desktop ALIAS qtcpp_desktop)

# 依赖注入发生在 main.cpp，因此具体实现 target 只由最终应用链接。
target_link_libraries(
    qtcpp_desktop
    PRIVATE
        QtCpp::application
        QtCpp::build_config
        QtCpp::gui
        QtCpp::platform
        QtCpp::project_options
        QtCpp::project_warnings
)

# 授权功能关闭时，编译器不会看到第三方 SDK 的任何类型或代码路径。
if(QTCPP_ENABLE_LICENSING)
    target_link_libraries(qtcpp_desktop PRIVATE QtCpp::registration_integration)
    target_compile_definitions(qtcpp_desktop PRIVATE QTCPP_ENABLE_LICENSING=1)
endif()

# 同一组版本变量同时驱动 Windows 资源和 macOS Bundle 元数据。
set_target_properties(
    qtcpp_desktop
    PROPERTIES
        OUTPUT_NAME "${QTCPP_APPLICATION_NAME}"
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "${QTCPP_APPLICATION_NAME}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "${QTCPP_BUNDLE_IDENTIFIER}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${QTCPP_VERSION_SEMVER}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${QTCPP_VERSION_BUILD}"
)

# 字体是运行时资源：macOS 放入 Bundle，Windows/Linux 放在程序旁的 fonts 目录。
if(APPLE)
    set(font_output_directory "$<TARGET_BUNDLE_CONTENT_DIR:qtcpp_desktop>/Resources/fonts")
else()
    set(font_output_directory "$<TARGET_FILE_DIR:qtcpp_desktop>/fonts")
endif()

add_custom_command(
    TARGET qtcpp_desktop
    POST_BUILD
    COMMAND
        "${CMAKE_COMMAND}" -E copy_directory
        "${PROJECT_SOURCE_DIR}/src/frontend/qt/assets/fonts"
        "${font_output_directory}"
    VERBATIM
)

# Windows 构建后只部署该程序实际依赖的 Qt DLL 和插件。
# 项目翻译已经编译进 qrc，无需复制整套 Qt 翻译文件。
if(WIN32)
    if(NOT TARGET Qt6::windeployqt)
        message(FATAL_ERROR "Qt Kit 中未找到 windeployqt，无法准备 bin 运行目录。")
    endif()
    add_custom_command(
        TARGET qtcpp_desktop
        POST_BUILD
        COMMAND
            "$<TARGET_FILE:Qt6::windeployqt>"
            --dir "$<TARGET_FILE_DIR:qtcpp_desktop>"
            --no-translations
            "$<TARGET_FILE:qtcpp_desktop>"
        COMMENT "Deploying required Qt runtime files to bin"
        VERBATIM
    )
endif()

# install() 生成可发布目录；项目根目录 bin 则用于本机直接运行和调试。
install(
    TARGETS qtcpp_desktop
    BUNDLE DESTINATION .
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

if(NOT APPLE)
    install(
        DIRECTORY "${PROJECT_SOURCE_DIR}/src/frontend/qt/assets/fonts/"
        DESTINATION "${CMAKE_INSTALL_BINDIR}/fonts"
        FILES_MATCHING PATTERN "*.ttf" PATTERN "LICENSE.txt"
    )
endif()

# 专有 SDK 的运行库由 third_party 模块按平台复制到正确位置。
if(QTCPP_ENABLE_LICENSING)
    qtcpp_install_registration_runtime(qtcpp_desktop)
endif()

# Qt 官方部署脚本在安装阶段补齐框架、插件和平台运行库。
set(qtcpp_deploy_tool_options)
if(APPLE AND NOT QTCPP_CODESIGN_IDENTITY STREQUAL "")
    list(APPEND qtcpp_deploy_tool_options "-codesign=${QTCPP_CODESIGN_IDENTITY}")
endif()

qt_generate_deploy_app_script(
    TARGET qtcpp_desktop
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_TOOL_OPTIONS ${qtcpp_deploy_tool_options}
)
install(SCRIPT "${deploy_script}")
