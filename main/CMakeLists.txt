# 生成
add_executable(${PROJECT_NAME}
    main.cpp
)

#qt_add_translations(${PROJECT_NAME} TS_FILES zh_CN.ts)

target_link_libraries(${PROJECT_NAME} PRIVATE
    app
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
