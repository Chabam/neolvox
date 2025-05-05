set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lvox_common)

target_sources(lvox_common
  PUBLIC
  src/logger/logger.cpp
  src/scanner/beam.cpp
  src/scanner/spherical_scanner.cpp
  src/voxel/grid.cpp
)

target_link_libraries(lvox_common
  PUBLIC
  pdalcpp
  Eigen3::Eigen
)

target_include_directories(lvox_common
  PUBLIC
  ${neolvox_SOURCE_DIR}/include
)

add_executable(lvox)

target_sources(lvox
  PRIVATE
  src/main.cpp
)

target_link_libraries(lvox PUBLIC lvox_common)

set_target_properties(lvox
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
