# 执行程序生成路径
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/windows)
#windows系统动态库生成lib文件命令
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif(APPLE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/macos)
else(UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/linux)
endif()

set(CMAKE_AUTOUIC_SEARCH_PATHS ui ui/SuperAdmin ui/Admin ui/User)
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h ui/*.ui qrc/*.qrc)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)

set(ts_files translations/login_cn.ts translations/login_en.ts)

add_library(app SHARED
    ${srcs}
)

qt_add_translations(app
    SOURCE_TARGETS app
    TS_FILE_BASE app
    TS_FILE_DIR translations
    TS_FILES app
    SOURCES app
)

target_link_libraries(app PUBLIC
    Qt6::Widgets
    FramelessWidget
)

target_include_directories(app PUBLIC include)
