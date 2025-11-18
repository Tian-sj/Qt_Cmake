set(cur_name "")
get_filename_component(cur_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h include/*.hpp ui/*.ui qrc/*.qrc)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools Network)

set(ts_files translations/${cur_name}_zh_CN.ts translations/${cur_name}_en.ts)

add_library(${cur_name} SHARED
    ${srcs}
)

set(QT_I18N_SOURCE_LANGUAGE "en")

set(QT_I18N_TRANSLATED_LANGUAGES zh_CN)

qt_add_translations(${cur_name}
    SOURCE_TARGETS ${cur_name}
    TS_FILE_BASE ${cur_name}
    TS_FILE_DIR translations
    TS_FILES ${ts_files}
    SOURCES ${cur_name}
)

target_link_libraries(${cur_name} PRIVATE
    Qt6::Widgets
    Qt6::Network
    registration_code
    magic_enum
    precision_countdown
)

target_include_directories(${cur_name}
    PRIVATE
        include/private
    PUBLIC
        include/public
)

# set(RESOURCE_PATH "skin")
# set(QRC_FILE ${CMAKE_CURRENT_SOURCE_DIR/qrc/resource.qrc})
# add_resources()
