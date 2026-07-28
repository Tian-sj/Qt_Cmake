set(cppproject_qml_import_paths ${QML_IMPORT_PATH} "${CMAKE_CURRENT_BINARY_DIR}")
list(REMOVE_DUPLICATES cppproject_qml_import_paths)
set(QML_IMPORT_PATH "${cppproject_qml_import_paths}" CACHE STRING
    "QML module search paths used by IDE tooling" FORCE)

qt_add_executable(cppproject_desktop_qml
    main.cpp
)

add_executable(CppProject::desktop_qml ALIAS cppproject_desktop_qml)

set_target_properties(cppproject_desktop_qml PROPERTIES AUTOMOC ON)
qt_policy(SET QTP0001 NEW)
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_qml_module(
    cppproject_desktop_qml
    URI CppProject.Desktop
    VERSION 1.0
    SOURCES
        qwindowkit_window_agent.cpp
        qwindowkit_window_agent.hpp
    QML_FILES
        qml/Main.qml
)

target_link_libraries(
    cppproject_desktop_qml
    PRIVATE
        CppProject::app_core
        CppProject::build_config
        CppProject::project_options
        CppProject::project_warnings
        CppProject::qwindowkit_quick
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
)

set_target_properties(
    cppproject_desktop_qml
    PROPERTIES
        OUTPUT_NAME "${CPPPROJECT_APPLICATION_NAME}Qml"
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "${CPPPROJECT_APPLICATION_NAME} QML"
        MACOSX_BUNDLE_GUI_IDENTIFIER "${CPPPROJECT_BUNDLE_IDENTIFIER}.qml"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${CPPPROJECT_VERSION_SEMVER}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${CPPPROJECT_VERSION_BUILD}"
)

if(CPPPROJECT_BUILD_TESTS)
    add_test(
        NAME cppproject_desktop_qml_smoke_test
        COMMAND "$<TARGET_FILE:cppproject_desktop_qml>" --smoke-test
    )
    set_tests_properties(
        cppproject_desktop_qml_smoke_test
        PROPERTIES
            ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software"
            LABELS qml
            TIMEOUT 15
    )
endif()

install(
    TARGETS cppproject_desktop_qml
    BUNDLE DESTINATION .
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

set(cppproject_qml_deploy_tool_options)
if(APPLE AND NOT CPPPROJECT_CODESIGN_IDENTITY STREQUAL "")
    list(APPEND cppproject_qml_deploy_tool_options
        "-codesign=${CPPPROJECT_CODESIGN_IDENTITY}")
endif()

qt_generate_deploy_qml_app_script(
    TARGET cppproject_desktop_qml
    OUTPUT_SCRIPT deploy_qml_script
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_TOOL_OPTIONS ${cppproject_qml_deploy_tool_options}
)
install(SCRIPT "${deploy_qml_script}")
