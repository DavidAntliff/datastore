cmake_minimum_required(VERSION 3.5.1)
project(datastore)

## Locate GTest
#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

enable_testing()
add_subdirectory(googletest)

include_directories(${CMAKE_SOURCE_DIR}/googletest/include)
add_executable(test_datastore datastore.c test_datastore.cpp)
target_link_libraries(test_datastore gtest_main gtest pthread bsd)
#target_link_libraries(test_datastore ${GTEST_LIBRARIES} pthread)

