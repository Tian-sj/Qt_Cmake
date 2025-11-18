# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.26)

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

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 指定工程名
project(${_NAME} LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(WIN32)
    set(OS_BIN_SUBDIR "bin/win")
    #windows系统动态库生成lib文件命令
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    # 设置静态库文件目录
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/windows)
    # 可执行文件目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/windows)
elseif(APPLE)
    set(OS_BIN_SUBDIR "bin/mac")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/macos)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OS_BIN_SUBDIR "bin/linux")
    set(PROGRAM_OUT_DIR "${CMAKE_SOURCE_DIR}/bin/linux")
    file(MAKE_DIRECTORY "${PROGRAM_OUT_DIR}")

    # 单配置生成器（Ninja/Makefile）
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${PROGRAM_OUT_DIR}") # 可执行
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${PROGRAM_OUT_DIR}") # SHARED/MODULE 默认也会来这里
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${PROGRAM_OUT_DIR}") # .a

    # 多配置生成器兼容（无害；在 Ninja 下也 OK）
    foreach(cfg Debug Release RelWithDebInfo MinSizeRel)
      string(TOUPPER "${cfg}" CFG_UP)
      set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFG_UP}  "${PROGRAM_OUT_DIR}")
      set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFG_UP}  "${PROGRAM_OUT_DIR}")
      set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFG_UP}  "${PROGRAM_OUT_DIR}")
    endforeach()

    # 运行时搜索“当前目录($ORIGIN)” + 你的私有运行库目录
    # 可执行：bin/linux/            -> $ORIGIN
    # 私有库：scripts/runtime/lib/   -> $ORIGIN/../../scripts/runtime/lib
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_INSTALL_RPATH
      "\$ORIGIN"                          # 找同目录的 .so（你自己生成的动态库）
      "\$ORIGIN/../../scripts/runtime/lib" # 找你随包带的新版 libstdc++
    )
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "" FORCE)
set(CMAKE_INSTALL_BINDIR "${OS_BIN_SUBDIR}"    CACHE PATH "" FORCE)
set(CMAKE_INSTALL_LIBDIR "${OS_BIN_SUBDIR}"    CACHE PATH "" FORCE)

add_subdirectory(src)
add_subdirectory(main)
# add_subdirectory(tests)
