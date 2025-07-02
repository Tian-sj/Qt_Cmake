# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.25)

get_filename_component(CUR_DIR_NAME
    "${CMAKE_CURRENT_SOURCE_DIR}"
    NAME
)

# 软件版本
set(_VERSION 0.0.1.250701_beta)
set(_DEVELOPER ChuanQi)
set(_NAME ${CUR_DIR_NAME})
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

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 指定工程名
project(${_NAME} LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# 执行程序生成路径
if(WIN32)
    #windows系统动态库生成lib文件命令
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    # 设置静态库文件目录
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/windows)
    # 可执行文件目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/windows)
elseif(APPLE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/macos)
else(UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/linux)
endif()


add_subdirectory(src)
add_subdirectory(main)
# add_subdirectory(tests)
