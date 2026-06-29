include_guard(GLOBAL)

function(qtcpp_resolve_git_version)
    set(version_major 1)
    set(version_minor 0)
    set(version_patch 0)
    set(version_build 0)
    set(git_hash "unknown")
    set(git_dirty FALSE)

    find_package(Git QUIET)
    if(Git_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" describe --abbrev=0 --tags
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE tag_result
            OUTPUT_VARIABLE git_tag
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if(tag_result EQUAL 0 AND git_tag MATCHES "^[vV]?([0-9]+)\\.([0-9]+)(\\.([0-9]+))?$")
            set(version_major "${CMAKE_MATCH_1}")
            set(version_minor "${CMAKE_MATCH_2}")
            if(CMAKE_MATCH_4 STREQUAL "")
                set(version_patch 0)
            else()
                set(version_patch "${CMAKE_MATCH_4}")
            endif()
        endif()

        execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-list --count HEAD
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE build_result
            OUTPUT_VARIABLE build_output
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(build_result EQUAL 0 AND build_output MATCHES "^[0-9]+$")
            set(version_build "${build_output}")
        endif()

        execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse --short=12 HEAD
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE hash_result
            OUTPUT_VARIABLE hash_output
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(hash_result EQUAL 0 AND NOT hash_output STREQUAL "")
            set(git_hash "${hash_output}")
        endif()

        execute_process(
            COMMAND "${GIT_EXECUTABLE}" status --porcelain --untracked-files=normal
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE dirty_result
            OUTPUT_VARIABLE dirty_output
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(dirty_result EQUAL 0 AND NOT dirty_output STREQUAL "")
            set(git_dirty TRUE)
        endif()
    endif()

    set(version_semver "${version_major}.${version_minor}.${version_patch}")
    set(version_full "${version_semver}.${version_build}")
    if(git_dirty)
        string(APPEND version_full "+dirty")
    endif()

    set(version_build_rc "${version_build}")
    if(version_build_rc GREATER 65535)
        math(EXPR version_build_rc "${version_build_rc} % 65536")
    endif()

    set(QTCPP_VERSION_MAJOR "${version_major}" PARENT_SCOPE)
    set(QTCPP_VERSION_MINOR "${version_minor}" PARENT_SCOPE)
    set(QTCPP_VERSION_PATCH "${version_patch}" PARENT_SCOPE)
    set(QTCPP_VERSION_BUILD "${version_build}" PARENT_SCOPE)
    set(QTCPP_VERSION_BUILD_RC "${version_build_rc}" PARENT_SCOPE)
    set(QTCPP_VERSION_SEMVER "${version_semver}" PARENT_SCOPE)
    set(QTCPP_VERSION_FULL "${version_full}" PARENT_SCOPE)
    set(QTCPP_GIT_HASH "${git_hash}" PARENT_SCOPE)
    set(QTCPP_GIT_DIRTY "${git_dirty}" PARENT_SCOPE)

    message(STATUS "Version: ${version_full} (${git_hash})")
endfunction()
