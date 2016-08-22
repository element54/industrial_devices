cmake_minimum_required (VERSION 2.6)
project (property_server)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

file(GLOB_RECURSE _HDRS "include/*.hpp")

find_package(Threads REQUIRED)
find_package(Boost 1.54.0 COMPONENTS regex system REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

add_library(property_server STATIC src/property.cpp src/device.cpp src/server.cpp)
target_include_directories(property_server PUBLIC include)
target_link_libraries(property_server ${Boost_LIBRARIES})
target_link_libraries(property_server ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(property_server jsoncpp)
target_link_libraries(property_server avahi_client)

#install(TARGETS property_server LIBRARY DESTINATION lib)
#install(FILES ${_HDRS} DESTINATION include/${PROJECT_NAME})
