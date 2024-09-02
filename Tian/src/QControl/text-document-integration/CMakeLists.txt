# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.5)

# 指定工程名
project(text_document_integration VERSION 0.1 LANGUAGES CXX)

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

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h ui/*.ui)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport)

add_library(text_document_integration SHARED
    ${srcs}
)

target_link_libraries(text_document_integration PRIVATE
    Qt6::Widgets
    Qt6::PrintSupport
    ${CMAKE_CURRENT_SOURCE_DIR}/../QCustomPlot/lib/libQCustomPlot.a
)

target_include_directories(text_document_integration PUBLIC
    include
    ../QCustomPlot/include
)

if(WIN32)
    set(LIB_PATH C:/Users/tian_sj/Desktop/Projects/ChuanQiHMI_bin)
elseif(APPLE)
    set(LIB_PATH /Users/tian_sj/Code/ChuanQiHMI/build/Debug/app)
endif()
add_custom_command(TARGET text_document_integration POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:text_document_integration>
    ${LIB_PATH}
)
