set(cur_name ct_test)

# 生成
add_executable(${cur_name}
    main.cpp
)

target_link_libraries(${cur_name} PRIVATE
    precision_countdown
)

# set_target_properties(${cur_name} PROPERTIES
#     WIN32_EXECUTABLE ON
#     MACOSX_BUNDLE ON
# )
