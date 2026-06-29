# 为所有轻量测试提供一致的编译策略、超时和 CTest 注册方式。
function(qtcpp_add_test target)
    add_executable("${target}" ${ARGN})

    # 测试可执行文件不是发布产物，单独存放以保持项目根目录 bin 干净。
    set_target_properties(
        "${target}"
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
            PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/symbols"
    )

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

# 测试目录镜像生产模块，便于按模块分工和定位失败。
qtcpp_add_test(qtcpp_application_tests application/application_service_tests.cpp)
target_link_libraries(qtcpp_application_tests PRIVATE QtCpp::application)

qtcpp_add_test(qtcpp_platform_tests platform/file_settings_store_tests.cpp)
target_link_libraries(qtcpp_platform_tests PRIVATE QtCpp::platform)

qtcpp_add_test(qtcpp_precision_countdown_tests precision_countdown/precision_countdown_tests.cpp)
target_link_libraries(qtcpp_precision_countdown_tests PRIVATE QtCpp::precision_countdown)
