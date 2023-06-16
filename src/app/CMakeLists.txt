qt_add_executable(${PROJECT_NAME}
  main.cpp
  main_window.cpp
  main_window.ui
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  test
  Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Core5Compat
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
