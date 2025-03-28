include(MyFindThirdParty)
include(MyCustomCommands)
include(MyFindQt6)

include_third_party(magic_enum nlohmann)

# 获取当前目录下的所有一级子目录
file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

# 遍历每个子目录，判断是否包含 CMakeLists.txt，然后添加
foreach(child ${children})
    if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
        if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
            message(STATUS "Adding subdirectory: ${child}")
            add_subdirectory(${child})
        endif()
    endif()
endforeach()