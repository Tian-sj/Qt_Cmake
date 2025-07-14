set(cur_name "")
get_filename_component(cur_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS include/*.hpp)

add_library(${cur_name} INTERFACE
    ${srcs}
)

target_include_directories(${cur_name} INTERFACE
    include
)
