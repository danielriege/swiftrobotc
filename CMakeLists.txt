cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS -pthread)

find_package(Boost COMPONENTS REQUIRED)

project(swiftrobotc)

file(GLOB PLIST_SOURCES src/Plist/*.cpp)
file(GLOB PLIST_HEADER src/Plist/*.hpp)
file(GLOB HEADER include/swiftrobotc/*.h)
file(GLOB SOURCES src/*.cpp)

include_directories( ${Boost_INCLUDE_DIRS} )

add_library(swiftrobotc SHARED
    ${SOURCES}
    ${HEADER}
    ${PLIST_SOURCES}
    ${PLIST_HEADER})
    
target_link_libraries(swiftrobotc ${Boost_LIBRARIES})
target_include_directories(swiftrobotc PRIVATE include/ src/Plist/)

add_executable(swiftrobotc_test
    ${SOURCES}
    ${HEADER}
    ${PLIST_SOURCES}
    ${PLIST_HEADER})
target_link_libraries(swiftrobotc_test ${Boost_LIBRARIES} ${OpenCV_LIBS})
target_include_directories(swiftrobotc_test PRIVATE include/ src/Plist/)

install(TARGETS swiftrobotc DESTINATION lib)
install(FILES ${HEADER} ${PLIST_HEADER} DESTINATION include/swiftrobotc)

