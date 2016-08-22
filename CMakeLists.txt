cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")
add_subdirectory(libs)
add_subdirectory(devices)
