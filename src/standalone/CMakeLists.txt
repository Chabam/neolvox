set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(liblvox)

target_sources(liblvox PRIVATE src/scanner/beam.cpp src/voxel/grid.cpp)

target_link_libraries(liblvox PUBLIC pdalcpp Eigen3::Eigen)

target_include_directories(liblvox PUBLIC ${neolvox_SOURCE_DIR}/include)

add_executable(lvox)

target_sources(lvox PRIVATE src/main.cpp)

target_link_libraries(lvox PUBLIC liblvox)

set_target_properties(lvox PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                      ${CMAKE_BINARY_DIR})

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
