# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.5)

# 指定工程名
project(NetTool VERSION 0.1 LANGUAGES CXX)

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

set(CMAKE_AUTOUIC_SEARCH_PATHS form/ui)
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS api/*.cpp form/*.cpp form/*.ui)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Core5Compat WebSockets LinguistTools)

set(ts_files translations/nettool_cn.ts translations/nettool_en.ts)

# add_library(NetTool SHARED
#     ${srcs}
# )
add_executable(NetTool
    main.cpp
    ${srcs}
)

qt_add_translations(NetTool
    SOURCE_TARGETS NetTool
    TS_FILE_BASE NetTool
    TS_FILE_DIR translations
    TS_FILES ${ts_files}
    SOURCES NetTool
)

target_link_libraries(NetTool PUBLIC
    Qt6::Widgets
    Qt6::Network
    Qt6::Core5Compat
    Qt6::WebSockets
)

target_include_directories(NetTool PUBLIC
    .
    api/
    form/
)

if(WIN32)
    set(LIB_PATH C:/Users/tian_sj/Desktop/Projects/ChuanQiHMI_bin)
    add_custom_command(TARGET NetTool POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:NetTool>
        ${LIB_PATH}
    )
endif()
