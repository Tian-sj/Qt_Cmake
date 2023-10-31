# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.5)

# 指定工程名
project(BTS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 执行程序生成路径
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/windows)
#windows系统动态库生成lib文件命令
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif(APPLE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/macos)
else(UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/linux)
endif()

# 查找 Qt 库
find_package(Qt5 REQUIRED COMPONENTS Widgets SerialPort Network PATHS /opt/homebrew/Cellar/qt@5/5.15.10_1)
#find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort Network LinguistTools PATHS /opt/homebrew/Cellar/qt/6.5.2)

# 设置 Boost 的路径
# set(BOOST_ROOT /opt/homebrew/Cellar/boost/1.83.0) # Boost 库的安装路径（根据你的实际安装路径设置）
# set(Boost_NO_SYSTEM_PATHS ON)  # 设置为 ON 表示不使用系统默认路径
# set(Boost_USE_STATIC_LIBS OFF)  # 设置为 OFF 表示使用动态链接库
# set(Boost_USE_MULTITHREADED ON)  # 设置为 ON 表示启用多线程支持

# find_package(Boost REQUIRED COMPONENTS thread )  # 指定你需要的 Boost 组件


# set(CMAKE_SYSTEM_NAME Linux)
# set(CMAKE_SYSTEM_PROCESSOR arm)

# # 指定交叉编译工具路径
# set(COMPILE /home/Work/U412/U41X-DEVKIT-1.0.0/1.GCCKIT/armgcc/bin)

# set(CMAKE_C_COMPILER ${COMPILE}/arm-linux-gcc)
# set(CMAKE_CXX_COMPILER ${COMPILE}/arm-linux-g++)

add_subdirectory(app)

# # 安装
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/linux)
# set(CMAKE_INSTALL_PREFIX "/opt/BTS")
# install(FILES ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}
#     DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
# )
