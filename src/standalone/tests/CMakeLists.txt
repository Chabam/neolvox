find_package(GTest 1.14.0 REQUIRED)

add_executable(lvox_tests)

target_sources(lvox_tests
    PRIVATE
    utils/utils.cpp
    voxel/test_grid_at.cpp
    voxel/test_grid_cell_count.cpp
    voxel/test_grid_create_from_point_cloud.cpp
    voxel/test_grid_creation_from_box3d.cpp
    voxel/test_grid_fixtures.hpp
    voxel/test_grid_index_of_point.cpp
    voxel/test_grid_traversal.cpp
    voxel/test_grid_voxel_bounds.cpp
    voxel/test_grid_voxel_bounds_from_point.cpp
)

target_include_directories(lvox_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lvox_tests
    GTest::GTest
    GTest::gtest_main
    pdalcpp
    Eigen3::Eigen
    lvox_common
)

target_include_directories(lvox_tests PUBLIC ${neolvox_SOURCE_DIR}/include)

set_target_properties(lvox_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

include(GoogleTest)
gtest_discover_tests(lvox_tests)
