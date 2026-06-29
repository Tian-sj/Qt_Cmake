# 后端始终可用，确保 core-only 模式完全不触发 Qt 配置。
add_subdirectory(backend)

# Qt 前端是可选表现层，关闭后仍可复用全部纯 C++ 模块。
if(QTCPP_BUILD_GUI)
    add_subdirectory(frontend)
endif()
