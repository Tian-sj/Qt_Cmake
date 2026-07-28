cmake_minimum_required(VERSION 3.25)

# 用法：cmake [-DFORCE=ON] [-DQT_ROOT=<路径>] [-DNINJA_PATH=<路径>]
#             [-DTHIRD_PARTY_ROOT=<共享第三方库路径>]
#             -P scripts/setup-toolchain.cmake
# 本脚本只发现本机工具并生成 CMakeUserPresets.json，不安装工具也不执行构建。
get_filename_component(project_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# OUTPUT 可用于测试或特殊工作区；默认文件由 Git 忽略，不会泄漏个人绝对路径。
if(NOT DEFINED OUTPUT OR OUTPUT STREQUAL "")
    set(OUTPUT "${project_root}/CMakeUserPresets.json")
endif()
get_filename_component(OUTPUT "${OUTPUT}" ABSOLUTE BASE_DIR "${project_root}")

if(EXISTS "${OUTPUT}" AND NOT FORCE)
    message(FATAL_ERROR "${OUTPUT} 已存在；如需覆盖请传入 -DFORCE=ON。")
endif()

# CMake 的 string(JSON) 需要已经带双引号并正确转义的 JSON 字符串值。
function(json_quote output value)
    string(REPLACE "\\" "\\\\" escaped "${value}")
    string(REPLACE "\"" "\\\"" escaped "${escaped}")
    string(REPLACE "\r" "\\r" escaped "${escaped}")
    string(REPLACE "\n" "\\n" escaped "${escaped}")
    string(REPLACE "\t" "\\t" escaped "${escaped}")
    set(${output} "\"${escaped}\"" PARENT_SCOPE)
endfunction()

# 接受“可执行文件”或“包含该程序的目录”，并统一成真实的 CMake 风格路径。
function(normalize_executable output candidate executable_name)
    if(candidate STREQUAL "")
        set(${output} "" PARENT_SCOPE)
        return()
    endif()

    file(TO_CMAKE_PATH "${candidate}" candidate_path)
    if(IS_DIRECTORY "${candidate_path}")
        set(candidate_path "${candidate_path}/${executable_name}")
    endif()
    if(EXISTS "${candidate_path}" AND NOT IS_DIRECTORY "${candidate_path}")
        get_filename_component(candidate_path "${candidate_path}" REALPATH)
        file(TO_CMAKE_PATH "${candidate_path}" candidate_path)
        set(${output} "${candidate_path}" PARENT_SCOPE)
    else()
        set(${output} "" PARENT_SCOPE)
    endif()
endfunction()

# 外部依赖根目录必须包含统一入口 third_party.cmake，不能把任意目录写入预设。
function(resolve_third_party_root output candidate)
    if(candidate STREQUAL "")
        set(${output} "" PARENT_SCOPE)
        return()
    endif()

    file(TO_CMAKE_PATH "${candidate}" candidate_path)
    if(IS_DIRECTORY "${candidate_path}" AND EXISTS "${candidate_path}/third_party.cmake")
        get_filename_component(candidate_path "${candidate_path}" REALPATH)
        file(TO_CMAKE_PATH "${candidate_path}" candidate_path)
        set(${output} "${candidate_path}" PARENT_SCOPE)
    else()
        set(${output} "" PARENT_SCOPE)
    endif()
endfunction()

# Qt 参数可以是安装前缀、Qt6Config.cmake 或 qmake/qtpaths 可执行文件。
function(resolve_qt_prefix output candidate)
    if(candidate STREQUAL "")
        set(${output} "" PARENT_SCOPE)
        return()
    endif()

    file(TO_CMAKE_PATH "${candidate}" candidate_path)
    if(EXISTS "${candidate_path}/lib/cmake/Qt6/Qt6Config.cmake")
        get_filename_component(candidate_path "${candidate_path}" REALPATH)
        set(${output} "${candidate_path}" PARENT_SCOPE)
        return()
    endif()

    if(EXISTS "${candidate_path}/Qt6Config.cmake")
        get_filename_component(candidate_path "${candidate_path}/../../.." ABSOLUTE)
        set(${output} "${candidate_path}" PARENT_SCOPE)
        return()
    endif()

    if(EXISTS "${candidate_path}" AND NOT IS_DIRECTORY "${candidate_path}")
        get_filename_component(tool_name "${candidate_path}" NAME)
        if(tool_name MATCHES "^(qtpaths6|qmake6|qmake)(\\.exe)?$")
            execute_process(
                COMMAND "${candidate_path}" -query QT_INSTALL_PREFIX
                RESULT_VARIABLE query_result
                OUTPUT_VARIABLE query_output
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if(query_result EQUAL 0)
                resolve_qt_prefix(resolved_prefix "${query_output}")
                set(${output} "${resolved_prefix}" PARENT_SCOPE)
                return()
            endif()
        endif()
    endif()

    set(${output} "" PARENT_SCOPE)
endfunction()

# 本机 configure preset 继承仓库预设，只补充工具的绝对路径和环境变量。
function(make_configure_preset output name parent ninja compiler qt_prefix environment)
    set(cache "{}")
    json_quote(value "${ninja}")
    string(JSON cache SET "${cache}" CMAKE_MAKE_PROGRAM "${value}")
    if(NOT compiler STREQUAL "")
        json_quote(value "${compiler}")
        string(JSON cache SET "${cache}" CMAKE_CXX_COMPILER "${value}")
    endif()
    if(NOT qt_prefix STREQUAL "")
        json_quote(value "${qt_prefix}")
        string(JSON cache SET "${cache}" CMAKE_PREFIX_PATH "${value}")
    endif()
    if(NOT external_third_party_root STREQUAL "")
        json_quote(value "${external_third_party_root}")
        string(JSON cache SET "${cache}" CPPPROJECT_EXTERNAL_THIRD_PARTY_ROOT "${value}")
    endif()

    set(preset "{}")
    json_quote(value "${name}")
    string(JSON preset SET "${preset}" name "${value}")
    json_quote(value "Local ${parent}")
    string(JSON preset SET "${preset}" displayName "${value}")
    json_quote(value "${parent}")
    string(JSON preset SET "${preset}" inherits "${value}")
    string(JSON preset SET "${preset}" cacheVariables "${cache}")
    if(NOT environment STREQUAL "{}")
        string(JSON preset SET "${preset}" environment "${environment}")
    endif()
    set(${output} "${preset}" PARENT_SCOPE)
endfunction()

# build/test preset 与同名 configure preset 绑定，避免重复配置路径。
function(make_build_preset output name)
    set(preset "{}")
    json_quote(value "${name}")
    string(JSON preset SET "${preset}" name "${value}")
    string(JSON preset SET "${preset}" configurePreset "${value}")
    set(${output} "${preset}" PARENT_SCOPE)
endfunction()

function(make_test_preset output name)
    set(preset "{}")
    json_quote(value "${name}")
    string(JSON preset SET "${preset}" name "${value}")
    string(JSON preset SET "${preset}" configurePreset "${value}")
    string(JSON preset SET "${preset}" output "{\"outputOnFailure\":true}")
    set(${output} "${preset}" PARENT_SCOPE)
endfunction()

# 按显式参数、环境变量、PATH 和常见安装目录的顺序查找 Ninja。
if(CMAKE_HOST_WIN32)
    set(ninja_name ninja.exe)
else()
    set(ninja_name ninja)
endif()

normalize_executable(ninja "${NINJA_PATH}" "${ninja_name}")
if(ninja STREQUAL "")
    normalize_executable(ninja "$ENV{NINJA_PATH}" "${ninja_name}")
endif()
if(ninja STREQUAL "")
    unset(ninja)
    find_program(
        ninja
        NAMES ninja ninja.exe
        PATHS
            "/opt/homebrew/bin"
            "/usr/local/bin"
            "C:/Program Files/CMake/bin"
            "C:/Qt/Tools/Ninja"
            "D:/Qt/Tools/Ninja"
            "C:/opt/Ninja"
            "D:/opt/Ninja"
        NO_CACHE
    )
endif()
if(NOT ninja)
    message(FATAL_ERROR "未找到 Ninja。请安装 Ninja，或传入 -DNINJA_PATH=<路径>。")
endif()
get_filename_component(ninja "${ninja}" REALPATH)
file(TO_CMAKE_PATH "${ninja}" ninja)

# Windows 必须捕获 VsDevCmd 提供的 MSVC/SDK 环境；Unix 只需确认编译器可用。
set(environment "{}")
set(preset_compiler "")
if(CMAKE_HOST_WIN32)
    find_program(
        vswhere
        NAMES vswhere.exe vswhere
        PATHS "C:/Program Files (x86)/Microsoft Visual Studio/Installer"
        NO_CACHE
    )
    if(NOT vswhere)
        message(FATAL_ERROR "未找到 vswhere；请安装 Visual Studio 2022 的 C++ 桌面开发组件。")
    endif()

    execute_process(
        COMMAND
            "${vswhere}" -latest -products * -version "[17.0,18.0)" -requires
            Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        RESULT_VARIABLE vs_result
        OUTPUT_VARIABLE vs_path
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ENCODING UTF-8
    )
    if(NOT vs_result EQUAL 0 OR vs_path STREQUAL "")
        message(FATAL_ERROR "未找到带 MSVC x64 工具链的 Visual Studio 2022。")
    endif()

    file(TO_CMAKE_PATH "${vs_path}" vs_path)
    set(vs_developer_command "${vs_path}/Common7/Tools/VsDevCmd.bat")
    if(NOT EXISTS "${vs_developer_command}")
        message(FATAL_ERROR "未找到 ${vs_developer_command}。")
    endif()

    # 使用短生命周期批处理文件，避免 cmd.exe 复合命令的多层引号解析问题。
    set(_msvc_env_bat "${project_root}/_msvc_env.bat")
    file(WRITE "${_msvc_env_bat}"
        "@echo off\r\n"
        "call \"${vs_developer_command}\" -no_logo -arch=x64 >nul\r\n"
        "set\r\n"
    )
    execute_process(
        COMMAND cmd.exe /d /c "${_msvc_env_bat}"
        RESULT_VARIABLE environment_result
        OUTPUT_VARIABLE environment_output
        ERROR_VARIABLE environment_error
        ENCODING UTF-8
    )
    if(NOT environment_result EQUAL 0)
        file(REMOVE "${_msvc_env_bat}")
        message(FATAL_ERROR "无法初始化 MSVC x64 环境：${environment_error}")
    endif()

    file(WRITE "${_msvc_env_bat}"
        "@echo off\r\n"
        "call \"${vs_developer_command}\" -no_logo -arch=x64 >nul\r\n"
        "where cl.exe\r\n"
    )
    execute_process(
        COMMAND cmd.exe /d /c "${_msvc_env_bat}"
        RESULT_VARIABLE compiler_result
        OUTPUT_VARIABLE compiler_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ENCODING UTF-8
    )
    file(REMOVE "${_msvc_env_bat}")
    if(NOT compiler_result EQUAL 0 OR compiler_output STREQUAL "")
        message(FATAL_ERROR "Visual Studio 环境已找到，但 cl.exe 不可用。")
    endif()

    string(REGEX MATCH "^[^\r\n]+" compiler "${compiler_output}")
    file(TO_CMAKE_PATH "${compiler}" compiler)
    set(preset_compiler "${compiler}")

    string(PREPEND environment_output "\n")
    foreach(variable IN ITEMS PATH INCLUDE LIB LIBPATH VCToolsInstallDir WindowsSdkDir)
        if(variable STREQUAL "PATH")
            set(variable_pattern "[Pp][Aa][Tt][Hh]")
        else()
            set(variable_pattern "${variable}")
        endif()
        string(REGEX MATCH "\n${variable_pattern}=([^\r\n]*)" match "${environment_output}")
        if(NOT match STREQUAL "")
            set(variable_value "${CMAKE_MATCH_1}")
            json_quote(value "${variable_value}")
            string(JSON environment SET "${environment}" "${variable}" "${value}")
        endif()
    endforeach()
else()
    normalize_executable(compiler "${CXX_PATH}" "c++")
    if(compiler STREQUAL "" AND NOT "$ENV{CXX}" STREQUAL "")
        normalize_executable(compiler "$ENV{CXX}" "c++")
    endif()
    if(compiler STREQUAL "")
        unset(compiler)
        find_program(compiler NAMES c++ clang++ g++ NO_CACHE)
    endif()
    if(NOT compiler)
        message(FATAL_ERROR "未找到 C++ 编译器；请安装 AppleClang、GCC 或 Clang。")
    endif()
    get_filename_component(compiler "${compiler}" REALPATH)
    file(TO_CMAKE_PATH "${compiler}" compiler)
endif()

# Qt 6 查找优先级：显式参数、环境变量、PATH 查询工具、常见安装目录。
set(qt_prefix "")
set(qt_candidates
    "${QT_ROOT}"
    "$ENV{QT_ROOT}"
    "$ENV{QT_ROOT_DIR}"
    "$ENV{QTDIR}"
    "$ENV{Qt6_DIR}"
    "$ENV{SDK_QT_PATH}"
)
find_program(qt_query_tool NAMES qtpaths6 qmake6 qmake NO_CACHE)
if(qt_query_tool)
    list(APPEND qt_candidates "${qt_query_tool}")
endif()

if(CMAKE_HOST_WIN32)
    file(GLOB common_qt_candidates LIST_DIRECTORIES TRUE
        "C:/Qt/6.*/*msvc*" "D:/Qt/6.*/*msvc*"
        "C:/opt/Qt/6.*/*msvc*" "D:/opt/Qt/6.*/*msvc*"
        "$ENV{USERPROFILE}/Qt/6.*/*msvc*")
elseif(CMAKE_HOST_APPLE)
    file(GLOB common_qt_candidates LIST_DIRECTORIES TRUE
        "/opt/Qt/6.*/macos" "$ENV{HOME}/Qt/6.*/macos" "/usr/local/Qt/6.*/macos")
else()
    file(GLOB common_qt_candidates LIST_DIRECTORIES TRUE
        "/opt/Qt/6.*/gcc_64" "$ENV{HOME}/Qt/6.*/gcc_64" "/usr/local/Qt/6.*/gcc_64")
endif()
list(SORT common_qt_candidates COMPARE NATURAL ORDER DESCENDING)
list(APPEND qt_candidates ${common_qt_candidates})

foreach(candidate IN LISTS qt_candidates)
    if(qt_prefix STREQUAL "")
        resolve_qt_prefix(candidate_prefix "${candidate}")
        if(NOT candidate_prefix STREQUAL "")
            set(qt_prefix "${candidate_prefix}")
        endif()
    endif()
endforeach()

if(CMAKE_HOST_WIN32 AND qt_prefix MATCHES "[Mm][Ii][Nn][Gg][Ww]")
    message(FATAL_ERROR "Windows 仅支持 MSVC Qt Kit，当前检测到的是 MinGW Kit。")
endif()

# 共享第三方库优先使用显式参数和环境变量，最后尝试项目同级 ../third_party。
set(external_third_party_root "")
set(third_party_candidates
    "${THIRD_PARTY_ROOT}"
    "$ENV{CPPPROJECT_THIRD_PARTY_ROOT}"
    "$ENV{THIRD_PARTY_ROOT}"
    "${project_root}/../third_party"
)
foreach(candidate IN LISTS third_party_candidates)
    if(external_third_party_root STREQUAL "")
        resolve_third_party_root(candidate_root "${candidate}")
        if(NOT candidate_root STREQUAL "")
            set(external_third_party_root "${candidate_root}")
        endif()
    endif()
endforeach()
if(NOT "${THIRD_PARTY_ROOT}" STREQUAL "" AND external_third_party_root STREQUAL "")
    message(FATAL_ERROR
        "THIRD_PARTY_ROOT 无效：${THIRD_PARTY_ROOT}；目录中必须存在 third_party.cmake。")
endif()

# 始终生成纯 C++ 预设；找到 Qt 时再生成 Qt 库、GUI 开发和发布预设。
set(configure_presets "[]")
set(build_presets "[]")
set(test_presets "[]")
set(preset_names local-core-only)

make_configure_preset(
    core_preset local-core-only core-only "${ninja}" "${preset_compiler}" "" "${environment}")
string(JSON configure_presets SET "${configure_presets}" 0 "${core_preset}")

if(NOT qt_prefix STREQUAL "")
    list(APPEND preset_names local-qt-libraries local-dev local-release)
    make_configure_preset(
        qt_libraries_preset
        local-qt-libraries
        qt-libraries
        "${ninja}"
        "${preset_compiler}"
        "${qt_prefix}"
        "${environment}"
    )
    make_configure_preset(
        dev_preset local-dev dev "${ninja}" "${preset_compiler}" "${qt_prefix}" "${environment}")
    make_configure_preset(
        release_preset local-release release "${ninja}" "${preset_compiler}" "${qt_prefix}" "${environment}")
    string(JSON configure_presets SET "${configure_presets}" 1 "${qt_libraries_preset}")
    string(JSON configure_presets SET "${configure_presets}" 2 "${dev_preset}")
    string(JSON configure_presets SET "${configure_presets}" 3 "${release_preset}")
endif()

set(index 0)
foreach(preset_name IN LISTS preset_names)
    make_build_preset(build_preset "${preset_name}")
    make_test_preset(test_preset "${preset_name}")
    string(JSON build_presets SET "${build_presets}" ${index} "${build_preset}")
    string(JSON test_presets SET "${test_presets}" ${index} "${test_preset}")
    math(EXPR index "${index} + 1")
endforeach()

# 直接使用 CMake JSON API 生成合法文件，避免依赖 Python 或平台特定模板替换。
set(presets "{}")
string(JSON presets SET "${presets}" version 6)
string(JSON presets SET "${presets}" configurePresets "${configure_presets}")
string(JSON presets SET "${presets}" buildPresets "${build_presets}")
string(JSON presets SET "${presets}" testPresets "${test_presets}")
file(WRITE "${OUTPUT}" "${presets}\n")

# 部分 CMake 环境不提供处理器名，此时使用平台原生环境变量或 uname 回退。
set(host_processor "${CMAKE_HOST_SYSTEM_PROCESSOR}")
if(host_processor STREQUAL "")
    if(CMAKE_HOST_WIN32)
        set(host_processor "$ENV{PROCESSOR_ARCHITECTURE}")
    else()
        execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE host_processor
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()
endif()

message(STATUS "平台：${CMAKE_HOST_SYSTEM_NAME} ${host_processor}")
message(STATUS "Ninja：${ninja}")
message(STATUS "C++ 编译器：${compiler}")
if(qt_prefix STREQUAL "")
    message(STATUS "Qt 6：未找到（仅生成纯 C++ 预设）")
else()
    message(STATUS "Qt 6：${qt_prefix}")
endif()
if(external_third_party_root STREQUAL "")
    message(STATUS "共享第三方库：未找到（需要时可传入 -DTHIRD_PARTY_ROOT=<路径>）")
else()
    message(STATUS "共享第三方库：${external_third_party_root}")
endif()
message(STATUS "已生成：${OUTPUT}")
