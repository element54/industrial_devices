cmake_minimum_required (VERSION 2.6)
project (test_device)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

add_executable(test_device src/run.cpp)
target_link_libraries(test_device property_server)

install(TARGETS test_device RUNTIME DESTINATION bin)
