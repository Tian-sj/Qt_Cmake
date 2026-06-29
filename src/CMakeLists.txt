add_subdirectory(backend)

if(QTCPP_BUILD_GUI)
    add_subdirectory(frontend)
endif()
