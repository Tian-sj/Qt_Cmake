set(cppproject_qt_dependency_configs Debug Release RelWithDebInfo MinSizeRel)
set(
    cppproject_qt_dependency_platform
    "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
)
set(
    cppproject_qt_dependency_toolchain
    "${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)
set(cppproject_qt_dependency_qt "Qt-${Qt6Core_VERSION}")
if(BUILD_SHARED_LIBS)
    set(cppproject_qt_dependency_linkage shared)
else()
    set(cppproject_qt_dependency_linkage static)
endif()

function(cppproject_qt_dependency_cache_root output dependency)
    set(cache_root
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/${dependency}/prebuilt/${cppproject_qt_dependency_platform}/${cppproject_qt_dependency_toolchain}/${cppproject_qt_dependency_qt}/${cppproject_qt_dependency_linkage}")
    set(${output} "${cache_root}" PARENT_SCOPE)
endfunction()

function(cppproject_write_qt_dependency_to_cache target dependency)
    cppproject_qt_dependency_cache_root(cache_root "${dependency}")
    foreach(config IN LISTS cppproject_qt_dependency_configs)
        string(TOUPPER "${config}" config_upper)
        set_target_properties(
            "${target}"
            PROPERTIES
                ARCHIVE_OUTPUT_DIRECTORY_${config_upper} "${cache_root}/${config}/lib"
                LIBRARY_OUTPUT_DIRECTORY_${config_upper} "${cache_root}/${config}/lib"
                RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${cache_root}/${config}/bin"
                PDB_OUTPUT_DIRECTORY_${config_upper} "${cache_root}/${config}/symbols"
        )
    endforeach()
endfunction()

function(cppproject_find_cached_qt_dependency output dependency)
    if(CMAKE_BUILD_TYPE STREQUAL "")
        message(FATAL_ERROR
            "Prebuilt Qt dependency mode requires CMAKE_BUILD_TYPE. "
            "Use Debug, Release, RelWithDebInfo, or MinSizeRel.")
    endif()

    cppproject_qt_dependency_cache_root(cache_root "${dependency}")
    set(config_root "${cache_root}/${CMAKE_BUILD_TYPE}")
    find_library(
        cached_library
        NAMES ${ARGN}
        PATHS "${config_root}/lib"
        NO_DEFAULT_PATH
        NO_CACHE
    )
    if(NOT cached_library)
        message(FATAL_ERROR
            "Prebuilt ${dependency} was not found in ${config_root}. "
            "Configure once with CPPPROJECT_BUILD_QT_THIRD_PARTY_FROM_SOURCE=ON "
            "and build the same configuration first.")
    endif()
    set(${output} "${cached_library}" PARENT_SCOPE)
endfunction()

if(CPPPROJECT_BUILD_QT_THIRD_PARTY_FROM_SOURCE)
    add_library(
        cppproject_qcustomplot
        vendor/qcustomplot/qcustomplot.cpp
        vendor/qcustomplot/qcustomplot.h
    )
    add_library(QCustomPlot::QCustomPlot ALIAS cppproject_qcustomplot)
    add_library(CppProject::qcustomplot ALIAS cppproject_qcustomplot)
    target_include_directories(
        cppproject_qcustomplot
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qcustomplot"
    )
    target_link_libraries(
        cppproject_qcustomplot
        PUBLIC
            Qt6::Core
            Qt6::Gui
            Qt6::PrintSupport
            Qt6::Widgets
    )
    set_target_properties(
        cppproject_qcustomplot
        PROPERTIES
            AUTOMOC ON
            OUTPUT_NAME qcustomplot
            POSITION_INDEPENDENT_CODE ON
    )
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(
            cppproject_qcustomplot
            PRIVATE QCUSTOMPLOT_COMPILE_LIBRARY
            PUBLIC QCUSTOMPLOT_USE_LIBRARY
        )
    endif()
    cppproject_write_qt_dependency_to_cache(cppproject_qcustomplot qcustomplot)

    file(
        GLOB cppproject_qxlsx_sources
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qxlsx/source/*.cpp"
    )
    file(
        GLOB cppproject_qxlsx_headers
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qxlsx/header/*.h"
    )
    add_library(
        cppproject_qxlsx
        ${cppproject_qxlsx_sources}
        ${cppproject_qxlsx_headers}
    )
    add_library(QXlsx::QXlsx ALIAS cppproject_qxlsx)
    add_library(CppProject::qxlsx ALIAS cppproject_qxlsx)
    target_include_directories(
        cppproject_qxlsx
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qxlsx/header"
    )
    target_link_libraries(
        cppproject_qxlsx
        PUBLIC
            Qt6::Core
            Qt6::Gui
    )
    if(TARGET Qt6::GuiPrivate)
        target_link_libraries(cppproject_qxlsx PRIVATE Qt6::GuiPrivate)
    endif()
    target_compile_definitions(
        cppproject_qxlsx
        PRIVATE
            QT_NO_KEYWORDS
            QT_NO_CAST_TO_ASCII
            QT_NO_CAST_FROM_ASCII
            QT_NO_URL_CAST_FROM_STRING
            QT_NO_CAST_FROM_BYTEARRAY
            QT_USE_QSTRINGBUILDER
            QT_NO_SIGNALS_SLOTS_KEYWORDS
            QT_USE_FAST_OPERATOR_PLUS
            QT_DISABLE_DEPRECATED_BEFORE=0x060600
    )
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(cppproject_qxlsx PUBLIC QXlsx_SHAREDLIB)
    endif()
    set_target_properties(
        cppproject_qxlsx
        PROPERTIES
            AUTOMOC ON
            OUTPUT_NAME QXlsxQt6
            POSITION_INDEPENDENT_CODE ON
    )
    cppproject_write_qt_dependency_to_cache(cppproject_qxlsx qxlsx)

    if(BUILD_SHARED_LIBS)
        set(QWINDOWKIT_BUILD_STATIC OFF CACHE BOOL "" FORCE)
    else()
        set(QWINDOWKIT_BUILD_STATIC ON CACHE BOOL "" FORCE)
    endif()
    set(QWINDOWKIT_BUILD_WIDGETS ON CACHE BOOL "" FORCE)
    set(QWINDOWKIT_BUILD_QUICK OFF CACHE BOOL "" FORCE)
    set(QWINDOWKIT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(QWINDOWKIT_BUILD_DOCUMENTATIONS OFF CACHE BOOL "" FORCE)
    set(QWINDOWKIT_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(vendor/qwindowkit qwindowkit)

    add_library(CppProject::qwindowkit ALIAS QWKWidgets)
    cppproject_write_qt_dependency_to_cache(QWKCore qwindowkit)
    cppproject_write_qt_dependency_to_cache(QWKWidgets qwindowkit)

    cppproject_qt_dependency_cache_root(qwindowkit_cache_root qwindowkit)
    add_custom_target(
        cppproject_cache_qwindowkit_headers
        ALL
        COMMAND
            "${CMAKE_COMMAND}" -E make_directory
            "${qwindowkit_cache_root}/$<CONFIG>/include/QWKCore"
            "${qwindowkit_cache_root}/$<CONFIG>/include/QWKWidgets"
        COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qwindowkit/src/core/qwkglobal.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qwindowkit/src/core/windowagentbase.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qwindowkit/src/core/qwindowkit_linux.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qwindowkit/src/core/qwindowkit_windows.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qwindowkit/src/core/style/styleagent.h"
            "${CMAKE_CURRENT_BINARY_DIR}/qwindowkit/etc/include/QWKCore/qwkconfig.h"
            "${qwindowkit_cache_root}/$<CONFIG>/include/QWKCore"
        COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qwindowkit/src/widgets/qwkwidgetsglobal.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qwindowkit/src/widgets/widgetwindowagent.h"
            "${qwindowkit_cache_root}/$<CONFIG>/include/QWKWidgets"
        DEPENDS QWKWidgets
        COMMENT "Caching QWindowKit public headers"
        VERBATIM
    )
else()
    cppproject_find_cached_qt_dependency(
        qcustomplot_library
        qcustomplot
        qcustomplot
    )
    if(WIN32 AND BUILD_SHARED_LIBS)
        add_library(cppproject_qcustomplot SHARED IMPORTED GLOBAL)
    else()
        add_library(cppproject_qcustomplot UNKNOWN IMPORTED GLOBAL)
    endif()
    add_library(QCustomPlot::QCustomPlot ALIAS cppproject_qcustomplot)
    add_library(CppProject::qcustomplot ALIAS cppproject_qcustomplot)
    set_target_properties(
        cppproject_qcustomplot
        PROPERTIES
            IMPORTED_LOCATION "${qcustomplot_library}"
            INTERFACE_INCLUDE_DIRECTORIES
                "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qcustomplot"
            INTERFACE_LINK_LIBRARIES
                "Qt6::Core;Qt6::Gui;Qt6::PrintSupport;Qt6::Widgets"
    )
    if(WIN32 AND BUILD_SHARED_LIBS)
        cppproject_qt_dependency_cache_root(qcustomplot_cache_root qcustomplot)
        find_file(
            qcustomplot_runtime
            NAMES qcustomplot.dll
            PATHS "${qcustomplot_cache_root}/${CMAKE_BUILD_TYPE}/bin"
            NO_DEFAULT_PATH
            NO_CACHE
            REQUIRED
        )
        set_target_properties(
            cppproject_qcustomplot
            PROPERTIES
                IMPORTED_IMPLIB "${qcustomplot_library}"
                IMPORTED_LOCATION "${qcustomplot_runtime}"
        )
    endif()
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(cppproject_qcustomplot INTERFACE QCUSTOMPLOT_USE_LIBRARY)
    endif()

    cppproject_find_cached_qt_dependency(qxlsx_library qxlsx QXlsxQt6)
    if(WIN32 AND BUILD_SHARED_LIBS)
        add_library(cppproject_qxlsx SHARED IMPORTED GLOBAL)
    else()
        add_library(cppproject_qxlsx UNKNOWN IMPORTED GLOBAL)
    endif()
    add_library(QXlsx::QXlsx ALIAS cppproject_qxlsx)
    add_library(CppProject::qxlsx ALIAS cppproject_qxlsx)
    set_target_properties(
        cppproject_qxlsx
        PROPERTIES
            IMPORTED_LOCATION "${qxlsx_library}"
            INTERFACE_INCLUDE_DIRECTORIES
                "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qxlsx/header"
            INTERFACE_LINK_LIBRARIES "Qt6::Core;Qt6::Gui"
    )
    if(WIN32 AND BUILD_SHARED_LIBS)
        cppproject_qt_dependency_cache_root(qxlsx_cache_root qxlsx)
        find_file(
            qxlsx_runtime
            NAMES QXlsxQt6.dll
            PATHS "${qxlsx_cache_root}/${CMAKE_BUILD_TYPE}/bin"
            NO_DEFAULT_PATH
            NO_CACHE
            REQUIRED
        )
        set_target_properties(
            cppproject_qxlsx
            PROPERTIES
                IMPORTED_IMPLIB "${qxlsx_library}"
                IMPORTED_LOCATION "${qxlsx_runtime}"
        )
    endif()
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(cppproject_qxlsx INTERFACE QXlsx_SHAREDLIB)
    endif()

    cppproject_find_cached_qt_dependency(
        qwindowkit_core_library
        qwindowkit
        QWKCore
        QWKCored
    )
    cppproject_find_cached_qt_dependency(
        qwindowkit_widgets_library
        qwindowkit
        QWKWidgets
        QWKWidgetsd
    )
    cppproject_qt_dependency_cache_root(qwindowkit_cache_root qwindowkit)
    set(qwindowkit_include_directory
        "${qwindowkit_cache_root}/${CMAKE_BUILD_TYPE}/include")
    if(NOT EXISTS "${qwindowkit_include_directory}/QWKWidgets/widgetwindowagent.h")
        message(FATAL_ERROR
            "Cached QWindowKit headers were not found in "
            "${qwindowkit_include_directory}. Rebuild QWindowKit from source.")
    endif()

    if(WIN32 AND BUILD_SHARED_LIBS)
        add_library(cppproject_qwindowkit_core SHARED IMPORTED GLOBAL)
    else()
        add_library(cppproject_qwindowkit_core UNKNOWN IMPORTED GLOBAL)
    endif()
    add_library(QWindowKit::Core ALIAS cppproject_qwindowkit_core)
    set_target_properties(
        cppproject_qwindowkit_core
        PROPERTIES
            IMPORTED_LOCATION "${qwindowkit_core_library}"
            INTERFACE_INCLUDE_DIRECTORIES "${qwindowkit_include_directory}"
            INTERFACE_LINK_LIBRARIES "Qt6::Core;Qt6::Gui"
    )

    if(WIN32 AND BUILD_SHARED_LIBS)
        add_library(cppproject_qwindowkit_widgets SHARED IMPORTED GLOBAL)
    else()
        add_library(cppproject_qwindowkit_widgets UNKNOWN IMPORTED GLOBAL)
    endif()
    add_library(QWindowKit::Widgets ALIAS cppproject_qwindowkit_widgets)
    add_library(CppProject::qwindowkit ALIAS cppproject_qwindowkit_widgets)
    set_target_properties(
        cppproject_qwindowkit_widgets
        PROPERTIES
            IMPORTED_LOCATION "${qwindowkit_widgets_library}"
            INTERFACE_INCLUDE_DIRECTORIES "${qwindowkit_include_directory}"
            INTERFACE_LINK_LIBRARIES
                "cppproject_qwindowkit_core;Qt6::Core;Qt6::Gui;Qt6::Widgets"
    )

    if(BUILD_SHARED_LIBS)
        if(WIN32)
            find_file(
                qwindowkit_core_runtime
                NAMES QWKCore.dll QWKCored.dll
                PATHS "${qwindowkit_cache_root}/${CMAKE_BUILD_TYPE}/bin"
                NO_DEFAULT_PATH
                NO_CACHE
                REQUIRED
            )
            find_file(
                qwindowkit_widgets_runtime
                NAMES QWKWidgets.dll QWKWidgetsd.dll
                PATHS "${qwindowkit_cache_root}/${CMAKE_BUILD_TYPE}/bin"
                NO_DEFAULT_PATH
                NO_CACHE
                REQUIRED
            )
            set_target_properties(
                cppproject_qwindowkit_core
                PROPERTIES
                    IMPORTED_IMPLIB "${qwindowkit_core_library}"
                    IMPORTED_LOCATION "${qwindowkit_core_runtime}"
            )
            set_target_properties(
                cppproject_qwindowkit_widgets
                PROPERTIES
                    IMPORTED_IMPLIB "${qwindowkit_widgets_library}"
                    IMPORTED_LOCATION "${qwindowkit_widgets_runtime}"
            )
        endif()
    else()
        target_compile_definitions(
            cppproject_qwindowkit_core
            INTERFACE QWK_CORE_STATIC
        )
        target_compile_definitions(
            cppproject_qwindowkit_widgets
            INTERFACE QWK_WIDGETS_STATIC
        )
        if(WIN32)
            target_link_libraries(cppproject_qwindowkit_core INTERFACE uxtheme)
        elseif(APPLE)
            target_link_libraries(
                cppproject_qwindowkit_core
                INTERFACE
                    "-framework Foundation"
                    "-framework Cocoa"
                    "-framework AppKit"
            )
        endif()
    endif()
endif()

if(CPPPROJECT_BUILD_TESTS)
    cppproject_add_test(
        cppproject_qt_third_party_tests
        tests/qt_third_party_tests.cpp
    )
    target_link_libraries(
        cppproject_qt_third_party_tests
        PRIVATE
            CppProject::qcustomplot
            CppProject::qwindowkit
            CppProject::qxlsx
    )
    set_tests_properties(
        cppproject_qt_third_party_tests
        PROPERTIES LABELS qt_third_party
    )
endif()
