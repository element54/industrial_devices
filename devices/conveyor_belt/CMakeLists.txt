cmake_minimum_required (VERSION 2.6)
project (conveyor_belt)

find_library (WIRINGPI_FOUND wiringPi)
if (WIRINGPI_FOUND)
    add_executable(conveyor_belt src/run.cpp)
    target_link_libraries(conveyor_belt property_server)
    target_link_libraries(conveyor_belt wiringPi)

    install(TARGETS conveyor_belt RUNTIME DESTINATION bin)
else()
    MESSAGE (WARNING "wiringPi NOT found. Skipping ${PROJECT_NAME}")
endif()
