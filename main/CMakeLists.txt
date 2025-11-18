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

# 把可执行安装到 bin/<os>（macOS 的 .app 也落这里）
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  BUNDLE  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 自定义部署脚本：先设目录 -> 再 include Qt 的部署支持 -> 再部署
qt_generate_deploy_script(
  TARGET ${PROJECT_NAME}
  OUTPUT_SCRIPT deploy_script
  CONTENT
"
# 把部署目标指向 bin/<os> 及其子目录
set(QT_DEPLOY_PREFIX          \"${CMAKE_SOURCE_DIR}\")
set(QT_DEPLOY_BIN_DIR         \"${OS_BIN_SUBDIR}\")
set(QT_DEPLOY_LIB_DIR         \"${OS_BIN_SUBDIR}\")
set(QT_DEPLOY_PLUGINS_DIR     \"${OS_BIN_SUBDIR}/plugins\")
set(QT_ENABLE_VERBOSE_DEPLOYMENT ON)

include(\"\${QT_DEPLOY_SUPPORT}\")

# 收集并复制 Qt 及非系统的运行时依赖到 bin/<os>
qt_deploy_runtime_dependencies(
  EXECUTABLE \"\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>\"
)

# Linux：把翻译文件放到 bin/linux/translations
if(UNIX AND NOT APPLE)
  set(QT_DEPLOY_TRANSLATIONS_DIR \"${OS_BIN_SUBDIR}/translations\")
  qt_deploy_translations(
    EXECUTABLE \"\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>\"
  )
endif()
"
)

# 安装时执行部署（等价于“Linux 版 windeployqt”）
install(SCRIPT ${deploy_script})
