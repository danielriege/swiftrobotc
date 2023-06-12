cmake_minimum_required (VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)

project (swiftrobotc)

add_subdirectory (src) 
add_subdirectory (test)

enable_testing ()
add_test (NAME test_swiftrobotc COMMAND swiftrobotc_test)
