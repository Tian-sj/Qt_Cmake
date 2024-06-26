file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(RegistrationCode SHARED
    ${srcs}
)

target_link_libraries(RegistrationCode PRIVATE
    Qt6::Core
)

target_include_directories(RegistrationCode PRIVATE include)
