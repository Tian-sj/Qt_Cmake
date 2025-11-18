set(cur_name cpp_test)

include(MyFindThirdParty)

include_third_party(soci)

find_package(SOCI REQUIRED COMPONENTS Core)

# 生成
add_executable(${cur_name}
    main.cpp
)

target_link_libraries(${cur_name} PRIVATE
    # server
    SOCI::Core
)

# set_target_properties(${cur_name} PROPERTIES
#     WIN32_EXECUTABLE ON
#     MACOSX_BUNDLE ON
# )
