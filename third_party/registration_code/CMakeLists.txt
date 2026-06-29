add_library(qtcpp_registration_sdk SHARED IMPORTED GLOBAL)
add_library(QtCpp::registration_sdk ALIAS qtcpp_registration_sdk)

set_target_properties(
    qtcpp_registration_sdk
    PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WIN32)
    set(qtcpp_registration_runtime "${CMAKE_CURRENT_SOURCE_DIR}/lib/win/registration_code.dll")
    set_target_properties(
        qtcpp_registration_sdk
        PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/win/registration_code.lib"
            IMPORTED_LOCATION "${qtcpp_registration_runtime}"
    )
elseif(APPLE)
    set(qtcpp_registration_runtime "${CMAKE_CURRENT_SOURCE_DIR}/lib/mac/libregistration_code.dylib")
    set_target_properties(
        qtcpp_registration_sdk
        PROPERTIES
            IMPORTED_LOCATION "${qtcpp_registration_runtime}"
    )
else()
    message(FATAL_ERROR
        "The proprietary registration SDK has no Linux binary. "
        "Configure with -DQTCPP_ENABLE_LICENSING=OFF or provide a Linux adapter."
    )
endif()

target_link_libraries(qtcpp_registration_sdk INTERFACE Qt6::Core)

function(qtcpp_install_registration_runtime app_target)
    get_target_property(registration_runtime qtcpp_registration_sdk IMPORTED_LOCATION)

    if(APPLE)
        set(registration_runtime_destination
            "$<TARGET_BUNDLE_CONTENT_DIR:${app_target}>/Frameworks")
    else()
        set(registration_runtime_destination "$<TARGET_FILE_DIR:${app_target}>")
    endif()

    add_custom_command(
        TARGET "${app_target}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${registration_runtime_destination}"
        COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:qtcpp_registration_sdk>"
            "${registration_runtime_destination}"
        VERBATIM
    )

    if(WIN32)
        install(FILES "${registration_runtime}" DESTINATION "${CMAKE_INSTALL_BINDIR}")
    endif()
endfunction()
