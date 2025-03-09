set(SDL_SHARED OFF CACHE BOOL "Disable shared lib")
set(SDL_STATIC ON CACHE BOOL "Build static lib")

add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_library(libs INTERFACE)
target_link_libraries(libs INTERFACE SDL3::SDL3)

