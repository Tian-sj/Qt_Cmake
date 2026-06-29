# 平台层实现 application 定义的存储和时钟等接口，不包含界面代码。
add_library(qtcpp_platform STATIC
    src/file_settings_store.cpp
    src/platform_paths.cpp
)

add_library(QtCpp::platform ALIAS qtcpp_platform)

# 公开实现类头文件，供 composition root 选择并注入具体实现。
target_include_directories(
    qtcpp_platform
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# PUBLIC application 表示公开头文件继承了其 ports；编译策略保持内部传播。
target_link_libraries(
    qtcpp_platform
    PUBLIC
        QtCpp::application
    PRIVATE
        QtCpp::project_options
        QtCpp::project_warnings
)

set_target_properties(qtcpp_platform PROPERTIES EXPORT_NAME platform)
