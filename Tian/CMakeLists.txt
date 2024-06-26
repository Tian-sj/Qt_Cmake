# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.5)

# 指定工程名
project(Tian VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_subdirectory(src/Communicator)
# add_subdirectory(src/CommProtocols)
add_subdirectory(src/QControl)
