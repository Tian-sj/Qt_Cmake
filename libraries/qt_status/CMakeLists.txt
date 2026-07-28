add_library(cppproject_qt_status)
add_library(CppProject::qt_status ALIAS cppproject_qt_status)

include(GenerateExportHeader)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/cppproject/qt_status")
generate_export_header(
    cppproject_qt_status
    BASE_NAME CPPPROJECT_QT_STATUS
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/include/cppproject/qt_status/export.hpp"
    STATIC_DEFINE CPPPROJECT_QT_STATUS_STATIC_DEFINE
)

target_sources(
    cppproject_qt_status
    PRIVATE
        src/status.cpp
    PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS
            include
            "${CMAKE_CURRENT_BINARY_DIR}/include"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/include/cppproject/qt_status/export.hpp"
            include/cppproject/qt_status/status.hpp
)

set_target_properties(
    cppproject_qt_status
    PROPERTIES
        AUTOMOC ON
        EXPORT_NAME qt_status
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
        POSITION_INDEPENDENT_CODE ON
)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(cppproject_qt_status PUBLIC CPPPROJECT_QT_STATUS_STATIC_DEFINE)
endif()

target_link_libraries(
    cppproject_qt_status
    PUBLIC
        Qt6::Core
    PRIVATE
        "$<BUILD_INTERFACE:CppProject::project_options>"
        "$<BUILD_INTERFACE:CppProject::project_warnings>"
)

install(
    TARGETS cppproject_qt_status
    EXPORT CppProjectTargets
    FILE_SET public_headers DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

if(CPPPROJECT_BUILD_TESTS)
    cppproject_add_test(
        cppproject_qt_status_tests
        tests/status_tests.cpp
    )
    target_link_libraries(cppproject_qt_status_tests PRIVATE CppProject::qt_status)
    set_tests_properties(cppproject_qt_status_tests PROPERTIES LABELS qt_status)
endif()
