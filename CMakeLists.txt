# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.5)

# 指定工程名
project(CQ-HMI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(app)
add_subdirectory(Tian)
add_subdirectory(main)
