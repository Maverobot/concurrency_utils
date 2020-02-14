cmake_minimum_required(VERSION 3.4)
project(
  realtime_utils
  VERSION 0.0.1
  LANGUAGES CXX)

find_package(Threads REQUIRED)

# Download catch2
include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.11.1)
FetchContent_GetProperties(catch2)
if(NOT catch2_POPULATED)
  FetchContent_Populate(catch2)
endif()
# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${catch2_SOURCE_DIR}/single_include)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${catch2_SOURCE_DIR}/contrib)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

add_library(realtime_utils INTERFACE)
target_include_directories(realtime_utils INTERFACE include)
target_link_libraries(realtime_utils INTERFACE Threads::Threads)

# Examples
add_executable(triple_buffer_example examples/triple_buffer_example.cpp)
target_link_libraries(triple_buffer_example realtime_utils Threads::Threads)
target_compile_features(triple_buffer_example PRIVATE cxx_std_14)

# Test
include(CTest)
include(Catch)
enable_testing()
set(TEST_SOURCES test/triple_buffer_test.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests realtime_utils Catch)
catch_discover_tests(tests)
