set(cur_name app)

# 执行程序生成路径
if(WIN32)
    #windows系统动态库生成lib文件命令
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    # 设置静态库文件目录
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    # 可执行文件目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/windows)
elseif(APPLE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/macos)
else(UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/linux)
endif()

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h ui/*.ui qrc/*.qrc)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)

set(ts_files translations/${cur_name}_cn.ts translations/${cur_name}_en.ts)

add_library(${cur_name} SHARED
    ${srcs}
)

qt_add_translations(${cur_name}
    SOURCE_TARGETS ${cur_name}
    TS_FILE_BASE ${cur_name}
    TS_FILE_DIR translations
    TS_FILES ${ts_files}
    SOURCES ${cur_name}
)

target_link_libraries(${cur_name} PUBLIC
    Qt6::Widgets
)

if(WIN32)
    target_link_libraries(${cur_name} PRIVATE
    )

elseif(APPLE)
    target_link_libraries(${cur_name} PRIVATE
    )
endif()

target_include_directories(${cur_name} PUBLIC
    include
)

if(WIN32)
    set(LIB_PATH C:/Users/tian_sj/Desktop/Projects/ChuanQiHMI_bin)
elseif(APPLE)
    set(LIB_PATH /Users/tian_sj/Code/Qt_CMake/build/Debug/app)
endif()
add_custom_command(TARGET ${cur_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:${cur_name}>
    ${LIB_PATH}
)
