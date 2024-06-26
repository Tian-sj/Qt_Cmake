# 执行程序生成路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

#windows系统动态库生成lib文件命令
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_library(CommProtocols SHARED
    ${srcs}
)

target_link_libraries(CommProtocols PRIVATE
    Communicator
)

target_include_directories(CommProtocols PRIVATE
    include
)
