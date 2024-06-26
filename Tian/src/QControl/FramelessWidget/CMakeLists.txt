file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(FramelessWidget SHARED
    ${srcs}
)

target_link_libraries(FramelessWidget PRIVATE
    Qt6::Widgets
)

target_include_directories(FramelessWidget PUBLIC
    include
)
