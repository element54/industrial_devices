cmake_minimum_required (VERSION 2.6)
project(avahi_client)

file(GLOB_RECURSE _HDRS "include/*.hpp")

find_package(Threads REQUIRED)

add_library(avahi_client STATIC src/avahi_client.cpp)
target_link_libraries(avahi_client avahi-client)
target_link_libraries(avahi_client avahi-common)
target_link_libraries(avahi_client ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(avahi_client PUBLIC include)

#install(TARGETS avahi_client LIBRARY DESTINATION lib)
#install(FILES ${_HDRS} DESTINATION include/${PROJECT_NAME})
