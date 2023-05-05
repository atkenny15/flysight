cmake_minimum_required(VERSION 3.24)
project(flysight_fw_tests)

include(CTest)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

find_package(Catch2 3 REQUIRED)

add_executable(common_test common_test.cc)
target_link_libraries(common_test PRIVATE Catch2::Catch2WithMain)
add_test(NAME libflysight_common_test COMMAND common_test)
