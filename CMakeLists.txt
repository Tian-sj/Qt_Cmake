# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.5)

# 软件版本
set(_VERSION 0.0.1.220829_beta)
set(_DEVELOPER Tian)
set(_NAME Qt_CMake)
set(_SUPPORT_URL )

if(UNIX)
    execute_process(
        COMMAND date +"%b %d %Y %H:%M:%S"
        OUTPUT_VARIABLE CURRENT_TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # 设置 BUILD_TIME 变量
    set(_BUILD_TIME ${CURRENT_TIME})
endif()

if(WIN32)
    execute_process(
        COMMAND powershell -Command "$Culture = [System.Globalization.CultureInfo]::GetCultureInfo(\"en-US\")\n(Get-Date).ToString('MMM dd yyyy HH:mm:ss', $Culture)"
        OUTPUT_VARIABLE CURRENT_TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # 设置 BUILD_TIME 变量
    set(_BUILD_TIME \"${CURRENT_TIME}\")
endif()

configure_file(${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_BINARY_DIR}/version.h)

include_directories(${CMAKE_BINARY_DIR})

# 指定工程名
project(${_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(app)
add_subdirectory(main)
# add_subdirectory(TEST)
