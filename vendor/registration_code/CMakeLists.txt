# 将预编译 SDK 包装成 imported target，调用方无需知道平台库文件路径。
add_library(cppproject_registration_sdk SHARED IMPORTED GLOBAL)
add_library(CppProject::registration_sdk ALIAS cppproject_registration_sdk)

set_target_properties(
    cppproject_registration_sdk
    PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Windows 链接 .lib 并运行时加载 .dll；macOS 直接链接 .dylib。
if(WIN32)
    set(cppproject_registration_runtime "${CMAKE_CURRENT_SOURCE_DIR}/lib/win/registration_code.dll")
    set_target_properties(
        cppproject_registration_sdk
        PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/win/registration_code.lib"
            IMPORTED_LOCATION "${cppproject_registration_runtime}"
    )
elseif(APPLE)
    set(cppproject_registration_runtime "${CMAKE_CURRENT_SOURCE_DIR}/lib/mac/libregistration_code.dylib")
    set_target_properties(
        cppproject_registration_sdk
        PROPERTIES
            IMPORTED_LOCATION "${cppproject_registration_runtime}"
    )
else()
    message(FATAL_ERROR
        "The proprietary registration SDK has no Linux binary. "
        "Configure with -DCPPPROJECT_ENABLE_LICENSING=OFF or provide a Linux adapter."
    )
endif()

# SDK 公共头暴露 Qt Core 类型，因此依赖只停留在 integration 边界内。
target_link_libraries(cppproject_registration_sdk INTERFACE Qt6::Core)

# 将 SDK 运行库复制到最终程序可加载的位置，并补充安装规则。
function(cppproject_install_registration_runtime app_target)
    get_target_property(registration_runtime cppproject_registration_sdk IMPORTED_LOCATION)

    if(APPLE)
        set(registration_runtime_destination
            "$<TARGET_BUNDLE_CONTENT_DIR:${app_target}>/Frameworks")
    else()
        set(registration_runtime_destination "$<TARGET_FILE_DIR:${app_target}>")
    endif()

    # copy_if_different 避免未变化的二进制触发无意义重写。
    add_custom_command(
        TARGET "${app_target}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${registration_runtime_destination}"
        COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:cppproject_registration_sdk>"
            "${registration_runtime_destination}"
        VERBATIM
    )

    if(WIN32)
        install(FILES "${registration_runtime}" DESTINATION "${CMAKE_INSTALL_BINDIR}")
    endif()
endfunction()
