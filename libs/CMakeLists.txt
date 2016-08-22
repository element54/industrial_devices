cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

add_subdirectory(avahi_client)
add_subdirectory(property_server)
