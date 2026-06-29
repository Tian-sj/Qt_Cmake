add_library(qtcpp_registration_integration STATIC
    src/registration_license_gateway.cpp
)

add_library(QtCpp::registration_integration ALIAS qtcpp_registration_integration)

target_include_directories(
    qtcpp_registration_integration
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_link_libraries(
    qtcpp_registration_integration
    PUBLIC
        QtCpp::application
    PRIVATE
        QtCpp::project_options
        QtCpp::project_warnings
        QtCpp::registration_sdk
)
