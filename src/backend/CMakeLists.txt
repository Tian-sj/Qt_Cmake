# application 先定义稳定接口，其他后端模块只能按该依赖方向接入。
add_subdirectory(application)

# 独立业务模块不依赖 application，可被 GUI、CLI 或服务程序单独复用。
add_subdirectory(modules/precision_countdown)

# platform 实现文件、时钟和操作系统路径等 application ports。
add_subdirectory(platform)

# 专有 SDK 默认不进入构建，避免影响 Linux 和纯 C++ 开发。
if(QTCPP_ENABLE_LICENSING)
    add_subdirectory(integrations/registration)
endif()
