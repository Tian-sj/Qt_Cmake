# 执行程序生成路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

#windows系统动态库生成lib文件命令
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

if(WIN32)
    set(BOOST_ROOT C:/boost/1.85.0)
elseif(APPLE)
    set(BOOST_ROOT /opt/homebrew/Cellar/boost/1.85.0)
else(UNIX)
endif()

set(Boost_USE_STATIC_LIBS ON)  # 设置为 OFF 表示使用动态链接库

add_library(Communicator SHARED
    ${srcs}
)

target_include_directories(Communicator PUBLIC
    ${BOOST_ROOT}/include
    include
)
