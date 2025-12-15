set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lvox_core)

target_sources(lvox_core
  PUBLIC
  src/algorithms/compute_pad.cpp
  src/logger/logger.cpp
  src/logger/progress.cpp
  src/scanner/beam.cpp
  src/scanner/spherical_scanner.cpp
  src/voxel/bounded_grid.cpp
  src/voxel/bounds.cpp
  src/voxel/chunked_grid.cpp
  src/voxel/coo_grid.cpp
  src/voxel/dense_grid.cpp
)

find_package(Eigen3 REQUIRED CONFIG)
find_package(TBB CONFIG)

target_link_libraries(lvox_core PUBLIC Eigen3::Eigen)

if (TBB_FOUND)
  target_link_libraries(lvox_core PUBLIC tbb)
endif()

target_include_directories(lvox_core
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(lvox_core PUBLIC $<$<CONFIG:Debug>:LVOX_DEBUG>)

add_library(lvox::core ALIAS lvox_core)
include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-msse4.2" COMPILER_SUPPORTS_SSE42)
if (COMPILER_SUPPORTS_SSE42)
    target_compile_options(lvox_core PRIVATE -msse4.2)
endif()

check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
if (COMPILER_SUPPORTS_AVX2)
    target_compile_options(lvox_core PRIVATE -mavx2)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
