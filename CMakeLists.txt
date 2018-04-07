cmake_minimum_required(VERSION 3.5.1)
project(datastore)

enable_testing()
add_subdirectory(googletest)

string(APPEND CMAKE_C_FLAGS " -std=gnu99")
string(APPEND CMAKE_C_FLAGS " -g -O0 --coverage")
string(APPEND CMAKE_EXE_LINKER_FLAGS " --coverage")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE=Debug)

set(LIBS gtest_main gtest pthread)

include_directories(${CMAKE_SOURCE_DIR}/googletest/include)
add_executable(test_datastore test_datastore.cpp datastore.c platform-posix.c string_to.c)
target_link_libraries(test_datastore ${LIBS})

# Custom target to run the tests
add_custom_target(run
  COMMAND test_datastore
  DEPENDS test_datastore
  WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

# Custom target to generate an HTML GCOV report
add_custom_target(report.html
  COMMAND gcovr -r . --root .. --html --html-details -o report.html
  DEPENDS run
  WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
