add_library(qtcpp_precision_countdown STATIC
    src/precision_countdown.cpp
)

add_library(QtCpp::precision_countdown ALIAS qtcpp_precision_countdown)

target_include_directories(
    qtcpp_precision_countdown
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(
    qtcpp_precision_countdown
    PRIVATE
        QtCpp::project_options
        QtCpp::project_warnings
)

set_target_properties(qtcpp_precision_countdown PROPERTIES EXPORT_NAME precision_countdown)
