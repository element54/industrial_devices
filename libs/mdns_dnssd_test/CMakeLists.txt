cmake_minimum_required (VERSION 2.6)
project (mdns_dnssd_test)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

add_executable(mdns_dnssd_test src/run.cpp)
target_link_libraries(mdns_dnssd_test avahi_client)

install(TARGETS mdns_dnssd_test RUNTIME DESTINATION bin)
