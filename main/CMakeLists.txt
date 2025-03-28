if(WIN32)
    set(APP_ICON "${CMAKE_CURRENT_BINARY_DIR}/app_win32.rc")
    file(WRITE ${APP_ICON}
        "IDI_ICON1 ICON \"${CMAKE_SOURCE_DIR}/icons/logo.ico\"\n"
    )
elseif(APPLE)
    # 设置 MACOSX_BUNDLE_ICON_FILE，只要是文件名，不要路径
    set(MACOSX_BUNDLE_ICON_FILE logo.icns)

    # 设置资源文件路径
    set(MACOSX_BUNDLE_BUNDLE_ICON_PATH icons)

    # 添加资源文件到工程
    set(app_icon_mac "${CMAKE_SOURCE_DIR}/icons/logo.icns")
    set_source_files_properties(${app_icon_mac} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )

    set(APP_ICON ${app_icon_mac})
endif()

find_package(Qt6 REQUIRED COMPONENTS Core)

# 生成
qt_add_executable(${PROJECT_NAME}
    main.cpp
    ${APP_ICON}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    gui
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
