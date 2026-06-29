# 高精度倒计时是无 Qt、无 application 依赖的独立纯 C++ 模块。
add_library(qtcpp_precision_countdown STATIC
    src/precision_countdown.cpp
)

add_library(QtCpp::precision_countdown ALIAS qtcpp_precision_countdown)

# 公共 API 使用稳定的 qtcpp/precision_countdown include 前缀。
target_include_directories(
    qtcpp_precision_countdown
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# 模块只继承统一编译策略，不向调用者传播实现依赖。
target_link_libraries(
    qtcpp_precision_countdown
    PRIVATE
        QtCpp::project_options
        QtCpp::project_warnings
)

set_target_properties(qtcpp_precision_countdown PROPERTIES EXPORT_NAME precision_countdown)
