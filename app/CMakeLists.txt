set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h ui/*ui qrc/*.qrc)

# 生成
add_executable(${PROJECT_NAME}
    main/main.cpp
    ${srcs}
    ${qrc_FILES}
)

#qt_add_translations(${PROJECT_NAME} TS_FILES zh_CN.ts)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Widgets
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
