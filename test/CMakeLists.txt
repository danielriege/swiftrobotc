include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

file(GLOB TESTS *.cpp)

add_executable(
  swiftrobotc_test
  ${TESTS}
)

target_link_libraries(
    swiftrobotc_test
    GTest::gtest_main
    ${Boost_LIBRARIES}
    swiftrobotc
)

include(GoogleTest)
gtest_discover_tests(swiftrobotc_test)

