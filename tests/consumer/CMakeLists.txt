cmake_minimum_required(VERSION 3.25)

project(CppProjectTemplateConsumer LANGUAGES CXX)

enable_testing()
find_package(CppProjectTemplate CONFIG REQUIRED)

add_executable(cppproject_package_consumer main.cpp)
target_compile_features(cppproject_package_consumer PRIVATE cxx_std_20)
target_link_libraries(
    cppproject_package_consumer
    PRIVATE
        CppProject::app_core
        CppProject::plugin_api
        CppProject::precision_countdown
)

add_test(NAME cppproject_package_consumer COMMAND cppproject_package_consumer)
