set(cur_name "")
get_filename_component(cur_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(${cur_name} SHARED IMPORTED GLOBAL)

# 让 include 正确传播
set_target_properties(${cur_name} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
)

if (WIN32)
    set_target_properties(${cur_name} PROPERTIES
            IMPORTED_IMPLIB  "${CMAKE_CURRENT_LIST_DIR}/lib/win/registration_code.lib"
            IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/win/registration_code.dll"
    )
elseif (APPLE)
    set_target_properties(${cur_name} PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/mac/libregistration_code.dylib"
    )
endif()

target_link_libraries(${cur_name} INTERFACE Qt6::Core)
