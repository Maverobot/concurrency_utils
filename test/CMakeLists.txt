# Build tests
set(TEST_SOURCES tests_main.cpp triple_buffer_test.cpp lock_free_queue_test.cpp
                 executor_test.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests concurrency_utils Catch)
target_compile_features(tests PRIVATE cxx_std_14)

# Let ctest discover the tests
include(Catch)
catch_discover_tests(tests)

# Coverage
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
  set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
  include(CodeCoverage)
  setup_target_for_coverage(${PROJECT_NAME}_coverage tests coverage)

  set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
  set(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
endif()
