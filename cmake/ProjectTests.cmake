include_guard(GLOBAL)

function(cppproject_add_test target)
    add_executable("${target}" ${ARGN})
    target_link_libraries(
        "${target}"
        PRIVATE
            CppProject::project_options
            CppProject::project_warnings
    )
    target_include_directories("${target}" PRIVATE "${PROJECT_SOURCE_DIR}/tests")
    set_target_properties(
        "${target}"
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/tests/bin"
            PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/tests/symbols"
    )
    add_test(NAME "${target}" COMMAND "${target}")
    set_tests_properties("${target}" PROPERTIES TIMEOUT 10)
endfunction()
