# Test utils
add_library(lvox_test_utils)

target_sources(lvox_test_utils
  PRIVATE
  utils/utils.cpp
)

target_include_directories(lvox_test_utils
    PUBLIC
    ${neolvox_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(lvox_test_utils
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_libraries(lvox_test_utils PUBLIC
    Eigen3::Eigen
    lvox::core
)

# Tests
find_package(GTest 1.14.0 REQUIRED)

add_executable(lvox_tests)

target_sources(lvox_tests
    PRIVATE
    scanner/test_spherical_scanner.cpp
    voxel/test_grid_cell_count.cpp
    voxel/test_grid_create_from_point_cloud.cpp
    voxel/test_grid_creation_from_bounds.cpp
    voxel/test_grid_index_of_point.cpp
    voxel/test_grid_traversal.cpp
    voxel/test_grid_voxel_bounds.cpp
    voxel/test_grid_voxel_bounds_from_point.cpp
)

target_include_directories(lvox_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(lvox_tests
    GTest::GTest
    GTest::gtest_main
    lvox_test_utils
)

if (WIN32)
    target_compile_definitions(lvox_tests PRIVATE NOMINMAX)
endif()

target_include_directories(lvox_tests PUBLIC ${neolvox_SOURCE_DIR}/include)

set_target_properties(lvox_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

include(GoogleTest)
gtest_discover_tests(lvox_tests)

# Benchmarks
find_package(benchmark REQUIRED)

add_executable(lvox_benchs)

target_sources(lvox_benchs
    PRIVATE
    benchmarks/bench_point_cloud_size.cpp
    benchmarks/bench_job_count.cpp
    benchmarks/main.cpp
)

target_include_directories(lvox_benchs
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${lvox_tests_SOURCE_DIR}
)

target_link_libraries(lvox_benchs
    benchmark::benchmark
    lvox_test_utils
)

target_include_directories(lvox_benchs PUBLIC ${neolvox_SOURCE_DIR}/include)

set_target_properties(lvox_benchs
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
