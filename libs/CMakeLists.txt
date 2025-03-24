set(SDL_SHARED OFF CACHE BOOL "Disable shared lib")
set(SDL_STATIC ON CACHE BOOL "Build static lib")

add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(glaze)

add_library(libs INTERFACE)

# Ensure include directories are properly set
target_include_directories(libs INTERFACE glaze/include)

# Link dependencies to the interface library
target_link_libraries(libs INTERFACE SDL3::SDL3)
