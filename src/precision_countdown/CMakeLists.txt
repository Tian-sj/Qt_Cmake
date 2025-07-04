set(cur_name precision_countdown)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS include/*.hpp)

add_library(${cur_name} INTERFACE
    ${srcs}
)

target_include_directories(${cur_name} INTERFACE
    include
)
