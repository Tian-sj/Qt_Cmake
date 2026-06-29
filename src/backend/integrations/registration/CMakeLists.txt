# 防腐层负责把专有 SDK 的 Qt 类型转换为 application 的标准 C++ 类型。
add_library(qtcpp_registration_integration STATIC
    src/registration_license_gateway.cpp
)

add_library(QtCpp::registration_integration ALIAS qtcpp_registration_integration)

# 仅公开 integration 自己的头文件，第三方 SDK include 不向上传播。
target_include_directories(
    qtcpp_registration_integration
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

# application 契约需要公开；专有 SDK 和编译选项都是实现细节。
target_link_libraries(
    qtcpp_registration_integration
    PUBLIC
        QtCpp::application
    PRIVATE
        QtCpp::project_options
        QtCpp::project_warnings
        QtCpp::registration_sdk
)
