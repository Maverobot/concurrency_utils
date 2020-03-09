cmake_minimum_required(VERSION 3.11)
project(
  concurrency_utils
  VERSION 0.0.1
  LANGUAGES CXX)

option(ENABLE_TESTING "Enable testing for this project" ON)

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

# Install concurrency_utils
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/ConcurrencyUtils)

install(
  TARGETS concurrency_utils
  EXPORT concurrency_utils-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# This is required so that the exported target has the name ConcurrencyUtils and
# not concurrency_utils
set_target_properties(concurrency_utils PROPERTIES EXPORT_NAME ConcurrencyUtils)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export the targets to a script
install(
  EXPORT concurrency_utils-targets
  FILE ConcurrencyUtilsTargets.cmake
  NAMESPACE ConcurrencyUtils::
  DESTINATION ${INSTALL_CONFIGDIR})

# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/ConcurrencyUtilsConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/ConcurrencyUtilsConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/ConcurrencyUtilsConfig.cmake INSTALL_DESTINATION
  ${INSTALL_CONFIGDIR})

# Install the config, configversion and custom find modules
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ConcurrencyUtilsConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/ConcurrencyUtilsConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIGDIR})

# ##############################################################################

export(
  EXPORT concurrency_utils-targets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/ConcurrencyUtilsTargets.cmake
  NAMESPACE ConcurrencyUtils::)

# Register package in user's package registry
set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE ConcurrencyUtils)
