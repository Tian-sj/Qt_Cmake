set(app_icon)

if(WIN32)
    set(app_icon "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/app.rc.in" "${app_icon}" @ONLY)
elseif(APPLE)
    set(app_icon "${PROJECT_SOURCE_DIR}/src/frontend/qt/assets/icons/logo.icns")
    set_source_files_properties("${app_icon}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

qt_add_executable(qtcpp_desktop
    main.cpp
    ${app_icon}
)

add_executable(QtCpp::desktop ALIAS qtcpp_desktop)

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

if(QTCPP_ENABLE_LICENSING)
    target_link_libraries(qtcpp_desktop PRIVATE QtCpp::registration_integration)
    target_compile_definitions(qtcpp_desktop PRIVATE QTCPP_ENABLE_LICENSING=1)
endif()

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

if(QTCPP_ENABLE_LICENSING)
    qtcpp_install_registration_runtime(qtcpp_desktop)
endif()

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
