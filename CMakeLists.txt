cmake_minimum_required(VERSION 3.10)
project(fg_3d)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/libs)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)
