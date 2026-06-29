# 项目选项只创建一次，避免被子目录重复 include 后产生同名 target。
include_guard(GLOBAL)

# 用 INTERFACE target 传播语言标准、警告和分析器配置，避免修改全局编译标志。
function(qtcpp_create_project_options)
    add_library(qtcpp_project_options INTERFACE)
    add_library(QtCpp::project_options ALIAS qtcpp_project_options)
    target_compile_features(qtcpp_project_options INTERFACE cxx_std_20)

    add_library(qtcpp_project_warnings INTERFACE)
    add_library(QtCpp::project_warnings ALIAS qtcpp_project_warnings)

    # MSVC 与 GCC/Clang 使用等价但不完全相同的严格警告集合。
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

    # Sanitizer 仅在显式开启时传播到编译和链接阶段，发布构建默认不启用。
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

    # clang-tidy 通过 CMake 原生变量接入，确保所有后续 C++ target 都受检查。
    if(QTCPP_ENABLE_CLANG_TIDY)
        find_program(QTCPP_CLANG_TIDY_EXECUTABLE NAMES clang-tidy REQUIRED)
        set(CMAKE_CXX_CLANG_TIDY "${QTCPP_CLANG_TIDY_EXECUTABLE}" PARENT_SCOPE)
    endif()
endfunction()
