add_library (test STATIC cout_hello.cpp)

target_include_directories(test INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
