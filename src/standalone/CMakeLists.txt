set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lvox)

target_sources(lvox
    PRIVATE
    src/main.cpp
    src/scanner/beam.cpp
)

target_link_libraries(lvox
    PRIVATE
    pdalcpp
    Eigen3::Eigen
)

target_include_directories(lvox PUBLIC ${neolvox_SOURCE_DIR}/include)

set_target_properties(lvox
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
