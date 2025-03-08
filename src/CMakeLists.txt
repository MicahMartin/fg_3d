# Collect source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# Define the executable target
add_executable(fg_3d ${SOURCES})

target_link_libraries(fg_3d PRIVATE SDL3::SDL3)

target_include_directories(fg_3d PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/SDL/include
)

