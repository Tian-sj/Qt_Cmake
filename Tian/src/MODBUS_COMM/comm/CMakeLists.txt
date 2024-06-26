file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_library(comm STATIC ${srcs} )

target_link_libraries(comm PUBLIC
    Qt::SerialPort
    Qt::Network
    cache
)

target_include_directories(comm PUBLIC include)
