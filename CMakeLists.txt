cmake_minimum_required(VERSION 3.26)

project(neolvox VERSION 0.0.0 LANGUAGES CXX)

option("LVOX_COMPUTREE_BINDINGS" "Enables binding with the Computree's platform" ON)
option("LVOX_STANDALONE" "Enables building LVOX as a standalone executable" OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/external/dependencies.cmake)

add_subdirectory(src)
