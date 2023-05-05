cmake_minimum_required(VERSION 3.24)
project(flysight_fw_tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

find_package(Catch2 3 REQUIRED)
find_package(fmt REQUIRED)

add_compile_options(-Wno-volatile)

add_executable(common_test
    common_test.cc
    avr/io.c
    ../src/Config.cpp
    ../src/exit_finder.cpp
    ../src/Log.c
    ../src/UBX.cpp
    ../src/PosLeds.cpp
    ../src/position_leds.cpp
    ../src/Power.c
    ../src/Time.c
    ../src/Timer.cpp
    ../src/Tone.c
    uart.c
)
target_compile_definitions(common_test PUBLIC RAMEND=0x1000)
target_include_directories(common_test
    PRIVATE include
    PRIVATE ../src
)
target_link_libraries(common_test PRIVATE Catch2::Catch2WithMain fmt::fmt)
add_test(NAME libflysight_common_test COMMAND common_test)
