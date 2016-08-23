cmake_minimum_required (VERSION 2.6)
project (rotating_turntable)

find_library (WIRINGPI_FOUND wiringPi)
if (WIRINGPI_FOUND)
    add_executable(rotating_turntable src/run.cpp)
    target_link_libraries(rotating_turntable property_server)
    target_link_libraries(rotating_turntable wiringPi)

    install(TARGETS rotating_turntable RUNTIME DESTINATION bin)
else()
    MESSAGE (WARNING "wiringPi NOT found. Skipping ${PROJECT_NAME}")
endif()
