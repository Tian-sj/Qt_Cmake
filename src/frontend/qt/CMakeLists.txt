# 显式列出源码，使增删文件在代码评审中可见，也避免 GLOB 导致漏重新配置。
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

# Qt 元对象、Designer UI 和 qrc 代码均由 CMake 在构建目录自动生成。
set_target_properties(
    qtcpp_gui
    PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
        AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/views"
)

# views/widgets 是前端内部实现；组合根只能包含 include/ 下的公开入口。
target_include_directories(
    qtcpp_gui
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

# application 是前后端契约，Qt Widgets 和构建信息仅供 GUI 内部使用。
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

# 翻译编译进资源系统，运行时无需在 bin 旁额外分发 .qm 文件。
qt_add_translations(
    qtcpp_gui
    TS_FILES
        translations/gui_zh_CN.ts
    RESOURCE_PREFIX "/i18n"
    LUPDATE_OPTIONS -no-obsolete
)
