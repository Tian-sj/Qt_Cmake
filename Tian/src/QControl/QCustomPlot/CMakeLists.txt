# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.5)

# 指定工程名
project(QCustomPlot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 执行程序生成路径
if(WIN32)
    #windows系统动态库生成lib文件命令
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    # 设置静态库文件目录
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
    # 可执行文件目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
elseif(APPLE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
else(UNIX)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
endif()

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

find_package(Qt6 REQUIRED COMPONENTS Core Gui PrintSupport)

add_library(QCustomPlot STATIC
    ${srcs}
)

target_link_libraries(QCustomPlot PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::PrintSupport
)

target_include_directories(QCustomPlot PUBLIC include)

if(WIN32)
    set(LIB_PATH C:/Users/tian_sj/Desktop/Projects/ChuanQiHMI_bin)
    add_custom_command(TARGET QCustomPlot POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:QCustomPlot>
        ${LIB_PATH}
    )
endif()


