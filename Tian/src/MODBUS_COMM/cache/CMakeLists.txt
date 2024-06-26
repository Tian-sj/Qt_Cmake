file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_library(cache STATIC ${srcs})

target_link_libraries(cache PUBLIC
    Qt::Core
)

target_include_directories(cache PUBLIC include)
