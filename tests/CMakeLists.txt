function(qtcpp_add_test target)
    add_executable("${target}" ${ARGN})
    target_link_libraries(
        "${target}"
        PRIVATE
            QtCpp::project_options
            QtCpp::project_warnings
    )
    target_include_directories("${target}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    add_test(NAME "${target}" COMMAND "${target}")
    set_tests_properties("${target}" PROPERTIES TIMEOUT 10)
endfunction()

qtcpp_add_test(qtcpp_application_tests application/application_service_tests.cpp)
target_link_libraries(qtcpp_application_tests PRIVATE QtCpp::application)

qtcpp_add_test(qtcpp_platform_tests platform/file_settings_store_tests.cpp)
target_link_libraries(qtcpp_platform_tests PRIVATE QtCpp::platform)

qtcpp_add_test(qtcpp_precision_countdown_tests precision_countdown/precision_countdown_tests.cpp)
target_link_libraries(qtcpp_precision_countdown_tests PRIVATE QtCpp::precision_countdown)
