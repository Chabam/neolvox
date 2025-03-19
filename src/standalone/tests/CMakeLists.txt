find_package(GTest 1.14.0 REQUIRED)

add_executable(lvox_tests)

target_sources(lvox_tests
    PRIVATE
        voxel/grid_test.cpp
)

target_link_libraries(lvox_tests
	GTest::GTest
	GTest::gtest_main
    pdalcpp
)
target_include_directories(lvox_tests PUBLIC ${neolvox_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(lvox_tests)
