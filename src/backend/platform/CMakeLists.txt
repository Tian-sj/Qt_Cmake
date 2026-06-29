add_library(qtcpp_platform STATIC
    src/file_settings_store.cpp
    src/platform_paths.cpp
)

add_library(QtCpp::platform ALIAS qtcpp_platform)

target_include_directories(
    qtcpp_platform
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(
    qtcpp_platform
    PUBLIC
        QtCpp::application
    PRIVATE
        QtCpp::project_options
        QtCpp::project_warnings
)

set_target_properties(qtcpp_platform PROPERTIES EXPORT_NAME platform)
