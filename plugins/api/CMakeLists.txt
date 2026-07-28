add_library(cppproject_plugin_api INTERFACE)
add_library(CppProject::plugin_api ALIAS cppproject_plugin_api)

target_sources(
    cppproject_plugin_api
    INTERFACE
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS include
        FILES include/cppproject/plugin/plugin_api.h
)

set_target_properties(cppproject_plugin_api PROPERTIES EXPORT_NAME plugin_api)

install(
    TARGETS cppproject_plugin_api
    EXPORT CppProjectTargets
    FILE_SET public_headers DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
