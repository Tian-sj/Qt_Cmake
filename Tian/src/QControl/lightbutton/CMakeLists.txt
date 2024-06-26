file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(lightbutton SHARED
    ${srcs}
)

target_link_libraries(lightbutton PRIVATE
    Qt6::Widgets
)

target_include_directories(lightbutton PUBLIC
    include
)
