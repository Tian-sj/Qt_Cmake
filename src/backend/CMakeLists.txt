add_subdirectory(application)
add_subdirectory(modules/precision_countdown)
add_subdirectory(platform)

if(QTCPP_ENABLE_LICENSING)
    add_subdirectory(integrations/registration)
endif()
