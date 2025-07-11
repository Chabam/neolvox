set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lvox_common)

target_sources(lvox_common
  PUBLIC
  src/algorithms.cpp
  src/algorithms/pad_compute_methods.cpp
  src/logger/logger.cpp
  src/scanner/beam.cpp
  src/scanner/scan.cpp
  src/scanner/spherical_scanner.cpp
  src/scanner/trajectory.cpp
  src/voxel/h5_exporter.cpp
)

target_link_libraries(lvox_common
  PUBLIC
  pdalcpp
  Eigen3::Eigen
  ${HDF5_CXX_LIBRARIES}
)

target_include_directories(lvox_common
  PUBLIC
  ${neolvox_SOURCE_DIR}/include
)

target_compile_definitions(lvox_common PUBLIC $<$<CONFIG:Debug>:LVOX_DEBUG>)

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-msse4.2" COMPILER_SUPPORTS_SSE42)
if (COMPILER_SUPPORTS_SSE42)
    target_compile_options(lvox_common PRIVATE -msse4.2)
endif()

check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
if (COMPILER_SUPPORTS_AVX2)
    target_compile_options(lvox_common PRIVATE -mavx2)
endif()

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
