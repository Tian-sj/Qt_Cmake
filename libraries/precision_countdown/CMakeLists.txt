add_library(cppproject_precision_countdown)
add_library(CppProject::precision_countdown ALIAS cppproject_precision_countdown)

include(GenerateExportHeader)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/cppproject/precision_countdown")
generate_export_header(
    cppproject_precision_countdown
    BASE_NAME CPPPROJECT_PRECISION_COUNTDOWN
    EXPORT_FILE_NAME
        "${CMAKE_CURRENT_BINARY_DIR}/include/cppproject/precision_countdown/export.hpp"
    STATIC_DEFINE CPPPROJECT_PRECISION_COUNTDOWN_STATIC_DEFINE
)

target_sources(
    cppproject_precision_countdown
    PRIVATE
        src/precision_countdown.cpp
    PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS
            include
            "${CMAKE_CURRENT_BINARY_DIR}/include"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/include/cppproject/precision_countdown/export.hpp"
            include/cppproject/precision_countdown/precision_countdown.hpp
            include/cppproject/precision_countdown/signal.hpp
)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(
        cppproject_precision_countdown
        PUBLIC CPPPROJECT_PRECISION_COUNTDOWN_STATIC_DEFINE
    )
endif()

target_link_libraries(
    cppproject_precision_countdown
    PRIVATE
        "$<BUILD_INTERFACE:CppProject::project_options>"
        "$<BUILD_INTERFACE:CppProject::project_warnings>"
)

set_target_properties(
    cppproject_precision_countdown
    PROPERTIES
        EXPORT_NAME precision_countdown
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
        POSITION_INDEPENDENT_CODE ON
)

install(
    TARGETS cppproject_precision_countdown
    EXPORT CppProjectTargets
    FILE_SET public_headers DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

if(CPPPROJECT_BUILD_TESTS)
    cppproject_add_test(
        cppproject_precision_countdown_tests
        tests/precision_countdown_tests.cpp
    )
    target_link_libraries(cppproject_precision_countdown_tests PRIVATE CppProject::precision_countdown)
    set_tests_properties(
        cppproject_precision_countdown_tests
        PROPERTIES LABELS precision_countdown
    )
endif()
