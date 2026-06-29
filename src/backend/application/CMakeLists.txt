# 应用层只包含用例编排和抽象 ports，不依赖 Qt、平台 API 或第三方 SDK。
add_library(qtcpp_application STATIC
    src/application_service.cpp
)

add_library(QtCpp::application ALIAS qtcpp_application)

# BUILD_INTERFACE 服务当前工作区；INSTALL_INTERFACE 为后续导出安装包保留。
target_include_directories(
    qtcpp_application
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# 编译策略使用 PRIVATE，避免将警告参数强制传播给外部调用者。
target_link_libraries(
    qtcpp_application
    PRIVATE
        QtCpp::project_options
        QtCpp::project_warnings
)

# 导出名去掉内部 qtcpp_ 前缀，保持安装后的 target 命名简洁。
set_target_properties(qtcpp_application PROPERTIES EXPORT_NAME application)
