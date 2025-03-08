# Collect source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# Define the executable target
add_executable(fg_3d ${SOURCES})
