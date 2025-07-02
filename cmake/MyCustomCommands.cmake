# cmake/CustomCommands.cmake

function(add_post_build_copy TARGET_NAME DESTINATION_PATH)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${TARGET_NAME}>
        ${DESTINATION_PATH}
    )
endfunction()
# set(LIB_PATH C:/Users/tian_sj/Desktop/Projects/ChuanQiHMI_bin)
# add_post_build_copy(RegistrationCode ${LIB_PATH})

function(add_resources TARGET_NAME QRC_FILE RESOURCE_PATH)
    # 检查当前系统
    if (WIN32)
        set(Qt6_RCC_EXECUTABLE "C:/Qt/6.8.3/msvc2022_64/bin/rcc")
        set(FINAL_RESOURCE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${RESOURCE_PATH})
    elseif(APPLE)
        set(Qt6_RCC_EXECUTABLE "/opt/Qt/6.8.3/macos/libexec/rcc")
        set(FINAL_RESOURCE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.app/Contents/MacOS/${RESOURCE_PATH})
    else()
        message(FATAL_ERROR "Unsupported platform for resource generation.")
    endif()

    # 创建目标资源目录
    file(MAKE_DIRECTORY ${FINAL_RESOURCE_PATH})

    # 添加自定义命令生成资源文件
    add_custom_command(
        OUTPUT ${FINAL_RESOURCE_PATH}/resource.rcc
        COMMAND ${Qt6_RCC_EXECUTABLE} -binary -o ${FINAL_RESOURCE_PATH}/resource.rcc ${QRC_FILE}
        DEPENDS ${QRC_FILE}
        COMMENT "Generating resource file from ${QRC_FILE}"
    )

    # 添加自定义目标
    add_custom_target(${TARGET_NAME}_Resources ALL DEPENDS ${FINAL_RESOURCE_PATH}/resource.rcc)

    # 提示生成资源的路径
    message(STATUS "Resource files for ${TARGET_NAME} will be generated at ${FINAL_RESOURCE_PATH}.")
endfunction()
# set(RESOURCE_PATH "skin")
# set(QRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/qrc/resource.qrc)
# add_resources(MyApp ${QRC_FILE} ${RESOURCE_PATH})
