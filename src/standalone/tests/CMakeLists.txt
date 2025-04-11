find_package(GTest 1.14.0 REQUIRED)

add_executable(lvox_tests)

target_sources(lvox_tests
    PRIVATE
    voxel/grid_test.cpp
    utils/utils.cpp
)

target_include_directories(lvox_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lvox_tests
    GTest::GTest
    GTest::gtest_main
    pdalcpp
    Eigen3::Eigen
    liblvox
)

target_include_directories(lvox_tests PUBLIC ${neolvox_SOURCE_DIR}/include)

set_target_properties(lvox_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

include(GoogleTest)
gtest_discover_tests(lvox_tests)
