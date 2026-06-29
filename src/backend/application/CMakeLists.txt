add_library(qtcpp_application STATIC
    src/application_service.cpp
)

add_library(QtCpp::application ALIAS qtcpp_application)

target_include_directories(
    qtcpp_application
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(
    qtcpp_application
    PRIVATE
        QtCpp::project_options
        QtCpp::project_warnings
)

set_target_properties(qtcpp_application PROPERTIES EXPORT_NAME application)
