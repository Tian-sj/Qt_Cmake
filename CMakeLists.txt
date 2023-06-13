# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.5)

# 指定功成名
project(name VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 设置Qt工具链的环境变量
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置UIC的查找路径，UIC默认会在引用ui头文件的源代码路径下查找ui文件，而我们的ui文件存放在/ui文件夹下
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

# 头文件
include_directories(${PROJECT_SOURCE_DIR}/include)

# 查找Qt库
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core5Compat)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core5Compat)


# 将各个文件夹下的所有文件打包
FILE(GLOB_RECURSE ${PROJECT_NAME}_HEADER "include/*.h")
FILE(GLOB_RECURSE ${PROJECT_NAME}_SOURCES "src/*.cpp")
FILE(GLOB_RECURSE ${PROJECT_NAME}_UI "ui/*.ui")
#FILE(GLOB_RECURSE ${PROJECT_NAME}_QRC "qrc/*.qrc")

set(PROJECT_SOURCES
#    "qrc/app_win32.rc"
    ${${PROJECT_NAME}_QRC}
    ${${PROJECT_NAME}_UI}
    ${${PROJECT_NAME}_HEADER}
    ${${PROJECT_NAME}_SOURCES}
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Core5Compat
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
