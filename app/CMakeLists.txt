set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h ui/*.ui qrc/*.qrc)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(app SHARED
    ${srcs}
    #${qrc_FILES}
)

target_link_libraries(app PUBLIC
    Qt6::Widgets
)

target_include_directories(app PUBLIC include)
