cmake_minimum_required(VERSION 3.11)
project(
  concurrency_utils
  VERSION 0.0.1
  LANGUAGES CXX)

option(ENABLE_TESTING "Enable testing for this project" ON)

# Download catch2 and prepare it for usage
include(cmake/FetchCatch2.cmake)

# Create concurrency_utils library
find_package(Threads REQUIRED)
add_library(concurrency_utils INTERFACE)
target_include_directories(
  concurrency_utils
  INTERFACE $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(concurrency_utils INTERFACE Threads::Threads)

# Examples
add_executable(triple_buffer_example examples/triple_buffer_example.cpp)
target_link_libraries(triple_buffer_example concurrency_utils)
target_compile_features(triple_buffer_example PRIVATE cxx_std_14)

# Test
if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
