cmake_minimum_required(VERSION 3.4)
project(
  realtime_utils
  VERSION 0.0.1
  LANGUAGES CXX)

find_package(Threads REQUIRED)

add_executable(triple_buffer_example examples/triple_buffer_example.cpp)
target_include_directories(triple_buffer_example PRIVATE include)
target_link_libraries(triple_buffer_example Threads::Threads)
target_compile_features(triple_buffer_example PRIVATE cxx_std_14)
