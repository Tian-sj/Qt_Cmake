file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

find_package(Qt6 REQUIRED COMPONENTS Core Gui PrintSupport)

add_library(QCustomPlot SHARED
    ${srcs}
)

target_link_libraries(QCustomPlot PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::PrintSupport
)

target_include_directories(QCustomPlot PRIVATE include)
