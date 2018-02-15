cmake_minimum_required(VERSION 3.5.1)
project(datastore)

## Locate GTest
#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

enable_testing()
add_subdirectory(googletest)

string(APPEND CMAKE_C_FLAGS " -std=c99")
string(APPEND CMAKE_C_FLAGS " -g -O0 --coverage")
string(APPEND CMAKE_EXE_LINKER_FLAGS " --coverage")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE=Debug)

set(LIBS gtest_main gtest pthread)
if(NOT APPLE)
  list(APPEND LIBS bsd)
endif()

include_directories(${CMAKE_SOURCE_DIR}/googletest/include)
add_executable(test_datastore datastore.c test_datastore.cpp)
#target_link_libraries(test_datastore gtest_main gtest pthread bsd)
target_link_libraries(test_datastore ${LIBS})
#target_link_libraries(test_datastore ${GTEST_LIBRARIES} pthread)

