# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.5)

# 指定工程名
project(TIAN VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 Boost 的路径
# set(BOOST_ROOT /opt/homebrew/Cellar/boost/1.83.0) # Boost 库的安装路径（根据你的实际安装路径设置）
# set(Boost_USE_STATIC_LIBS ON)  # 设置为 OFF 表示使用动态链接库


add_subdirectory(app)
add_subdirectory(main)