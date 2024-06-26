file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_library(${PROJECT_NAME} SHARED ${srcs})

target_link_libraries(${PROJECT_NAME} PUBLIC
    comm
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
