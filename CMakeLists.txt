cmake_minimum_required(VERSION 3.26)

project(neolvox VERSION 0.0.0 LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/external/dependencies.cmake)
add_subdirectory(lvox)
