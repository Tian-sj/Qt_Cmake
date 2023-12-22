# 执行程序生成路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#windows系统动态库生成lib文件命令
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h ui/*.ui qrc/*.qrc)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(app SHARED
    ${srcs}
    #${qrc_FILES}
)

target_link_libraries(app PUBLIC
    Qt6::Widgets
)

target_include_directories(app PUBLIC include)
