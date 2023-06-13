
find_package(Boost COMPONENTS REQUIRED)

include_directories( ${Boost_INCLUDE_DIRS} )

file(GLOB PLIST_SOURCES ${PROJECT_SOURCE_DIR}/src/Plist/*.cpp)
file(GLOB PLIST_HEADER ${PROJECT_SOURCE_DIR}/srcPlist/*.hpp)
file(GLOB HEADER ${PROJECT_SOURCE_DIR}/include/swiftrobotc/*.h)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c)

add_library(swiftrobotc SHARED
    ${SOURCES}
    ${HEADER}
    ${PLIST_SOURCES}
    ${PLIST_HEADER})
    
target_link_libraries(swiftrobotc ${Boost_LIBRARIES})
target_include_directories(swiftrobotc PUBLIC ${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/src/Plist/)

add_executable(swiftrobotc_demo
    ${SOURCES}
    ${HEADER}
    ${PLIST_SOURCES}
    ${PLIST_HEADER})
target_link_libraries(swiftrobotc_demo ${Boost_LIBRARIES} ${OpenCV_LIBS})
target_include_directories(swiftrobotc_demo PRIVATE ${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/src/Plist/)

install(TARGETS swiftrobotc DESTINATION lib)
install(FILES ${HEADER} ${PLIST_HEADER} DESTINATION include/swiftrobotc)

