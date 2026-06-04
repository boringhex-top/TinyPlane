cmake_minimum_required(VERSION 3.16)

# We create an executable for running tests on host machine directly
project(tiny_plane_tests)

set(CMAKE_C_STANDARD 11)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../main
)

# Source files
set(SOURCES
    unity.c
    test_protocol.c
    test_flight_control.c
    test_main.c
    hardware_mock.c
    ../main/protocol.c
    ../main/flight_control.c
)

add_executable(run_tests ${SOURCES})

# Link math library
target_link_libraries(run_tests m)

# Hardware mock to avoid ESP-IDF dependencies in flight_control test
target_compile_definitions(run_tests PRIVATE -DUNIT_TEST_HOST)
