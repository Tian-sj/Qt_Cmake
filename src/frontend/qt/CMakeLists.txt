qt_add_library(qtcpp_gui STATIC
    include/qtcpp/gui/gui_application.hpp
    application/gui_application.cpp
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

add_library(QtCpp::gui ALIAS qtcpp_gui)

set_target_properties(
    qtcpp_gui
    PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
        AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/views"
)

target_include_directories(
    qtcpp_gui
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_link_libraries(
    qtcpp_gui
    PUBLIC
        QtCpp::application
    PRIVATE
        Qt6::Widgets
        QtCpp::build_config
        QtCpp::project_options
        QtCpp::project_warnings
)

qt_add_translations(
    qtcpp_gui
    TS_FILES
        translations/gui_zh_CN.ts
    RESOURCE_PREFIX "/i18n"
    LUPDATE_OPTIONS -no-obsolete
)
