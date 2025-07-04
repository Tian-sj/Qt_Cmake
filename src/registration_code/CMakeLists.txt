set(cur_name registration_code)

include(MyFindQt6)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h include/*.hpp)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(${cur_name} SHARED
    ${srcs}
)

target_link_libraries(${cur_name} PRIVATE
    Qt6::Core
)

target_include_directories(${cur_name} PUBLIC
    include
)

# set(LIB_PATH C:/Users/tian_sj/Desktop/Projects/ChuanQiHMI_bin)
# add_post_build_copy(${cur_name} ${LIB_PATH})
