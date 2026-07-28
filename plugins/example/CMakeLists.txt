add_library(cppproject_example_plugin MODULE src/example_plugin.cpp)
target_compile_definitions(cppproject_example_plugin PRIVATE CPPPROJECT_PLUGIN_IMPLEMENTATION=1)
target_link_libraries(
    cppproject_example_plugin
    PRIVATE
        CppProject::plugin_api
        CppProject::project_options
        CppProject::project_warnings
)
set_target_properties(
    cppproject_example_plugin
    PROPERTIES
        PREFIX ""
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
)

install(
    TARGETS cppproject_example_plugin
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/cppproject/plugins"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/plugins"
)

if(CPPPROJECT_BUILD_TESTS)
    add_executable(cppproject_example_plugin_load_tests tests/plugin_load_tests.cpp)
    target_link_libraries(
        cppproject_example_plugin_load_tests
        PRIVATE
            CppProject::plugin_api
            CppProject::project_options
            CppProject::project_warnings
            ${CMAKE_DL_LIBS}
    )
    set_target_properties(
        cppproject_example_plugin_load_tests
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/tests/bin"
            PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/tests/symbols"
    )
    add_test(
        NAME cppproject_example_plugin_load_tests
        COMMAND cppproject_example_plugin_load_tests "$<TARGET_FILE:cppproject_example_plugin>"
    )
    set_tests_properties(
        cppproject_example_plugin_load_tests
        PROPERTIES
            LABELS plugin
            TIMEOUT 10
    )
endif()
