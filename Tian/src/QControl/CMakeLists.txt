# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.5)

# 指定工程名
project(QControl VERSION 1.0 LANGUAGES CXX)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 执行程序生成路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

#windows系统动态库生成lib文件命令
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# add_subdirectory(lightbutton)
# add_subdirectory(QCustomPlot)
# add_subdirectory(RegistrationCode)
add_subdirectory(FramelessWidget)
