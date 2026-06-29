include_guard(GLOBAL)

function(qtcpp_create_project_options)
    add_library(qtcpp_project_options INTERFACE)
    add_library(QtCpp::project_options ALIAS qtcpp_project_options)
    target_compile_features(qtcpp_project_options INTERFACE cxx_std_20)

    add_library(qtcpp_project_warnings INTERFACE)
    add_library(QtCpp::project_warnings ALIAS qtcpp_project_warnings)

    if(MSVC)
        target_compile_options(
            qtcpp_project_warnings
            INTERFACE /W4 /permissive- /Zc:__cplusplus /utf-8
        )
        if(QTCPP_WARNINGS_AS_ERRORS)
            target_compile_options(qtcpp_project_warnings INTERFACE /WX)
        endif()
    else()
        target_compile_options(
            qtcpp_project_warnings
            INTERFACE
                -Wall
                -Wextra
                -Wpedantic
                -Wconversion
                -Wsign-conversion
                -Wshadow
        )
        if(QTCPP_WARNINGS_AS_ERRORS)
            target_compile_options(qtcpp_project_warnings INTERFACE -Werror)
        endif()
    endif()

    if(QTCPP_ENABLE_SANITIZERS)
        if(MSVC)
            target_compile_options(qtcpp_project_options INTERFACE /fsanitize=address)
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            target_compile_options(qtcpp_project_options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(qtcpp_project_options INTERFACE -fsanitize=address,undefined)
        else()
            message(WARNING "Sanitizers are not configured for ${CMAKE_CXX_COMPILER_ID}")
        endif()
    endif()

    if(QTCPP_ENABLE_CLANG_TIDY)
        find_program(QTCPP_CLANG_TIDY_EXECUTABLE NAMES clang-tidy REQUIRED)
        set(CMAKE_CXX_CLANG_TIDY "${QTCPP_CLANG_TIDY_EXECUTABLE}" PARENT_SCOPE)
    endif()
endfunction()
